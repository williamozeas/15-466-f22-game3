#include "PlayMode.hpp"

#include "LitColorTextureProgram.hpp"

#include "DrawLines.hpp"
#include "Mesh.hpp"
#include "gl_errors.hpp"
#include "data_path.hpp"

#include <glm/gtc/type_ptr.hpp>

#include <random>

GLuint meshes_for_lit_color_texture_program = 0;
Load< MeshBuffer > meshes(LoadTagDefault, []() -> MeshBuffer const * {
	MeshBuffer const *ret = new MeshBuffer(data_path("ping_pong.pnct"));
    meshes_for_lit_color_texture_program = ret->make_vao_for_program(lit_color_texture_program->program);
	return ret;
});

Load< Scene > ping_pong_scene(LoadTagDefault, []() -> Scene const * {
	return new Scene(data_path("ping_pong.scene"), [&](Scene &scene, Scene::Transform *transform, std::string const &mesh_name){
		Mesh const &mesh = meshes->lookup(mesh_name);

		scene.drawables.emplace_back(transform);
		Scene::Drawable &drawable = scene.drawables.back();

		drawable.pipeline = lit_color_texture_program_pipeline;

		drawable.pipeline.vao = meshes_for_lit_color_texture_program;
		drawable.pipeline.type = mesh.type;
		drawable.pipeline.start = mesh.start;
		drawable.pipeline.count = mesh.count;

	});
});

Load< Sound::Sample > ping_pong_loop(LoadTagDefault, []() -> Sound::Sample const * {
	return new Sound::Sample(data_path("audio/Ping Pong Loop.opus"));
});

Load< Sound::Sample > ping_pong_bounce_01(LoadTagDefault, []() -> Sound::Sample const * {
    return new Sound::Sample(data_path("audio/PingPongBounce01.opus"));
});

Load< Sound::Sample > ping_pong_bounce_02(LoadTagDefault, []() -> Sound::Sample const * {
    return new Sound::Sample(data_path("audio/PingPongBounce02.opus"));
});

Load< Sound::Sample > ping_pong_bounce_03(LoadTagDefault, []() -> Sound::Sample const * {
    return new Sound::Sample(data_path("audio/PingPongBounce03.opus"));
});

Load< Sound::Sample > ping_pong_bounce_04(LoadTagDefault, []() -> Sound::Sample const * {
    return new Sound::Sample(data_path("audio/PingPongBounce04.opus"));
});

Load< Sound::Sample > ping_pong_drop(LoadTagDefault, []() -> Sound::Sample const * {
    return new Sound::Sample(data_path("audio/PingPongDrop01.opus"));
});

Load< Sound::Sample > ping_pong_hi_big(LoadTagDefault, []() -> Sound::Sample const * {
    return new Sound::Sample(data_path("audio/PingPongHitHiBig.opus"));
});

Load< Sound::Sample > ping_pong_hi(LoadTagDefault, []() -> Sound::Sample const * {
    return new Sound::Sample(data_path("audio/PingPongHitHi.opus"));
});

Load< Sound::Sample > ping_pong_low_big(LoadTagDefault, []() -> Sound::Sample const * {
    return new Sound::Sample(data_path("audio/PingPongHitLowBig.opus"));
});

Load< Sound::Sample > ping_pong_low(LoadTagDefault, []() -> Sound::Sample const * {
    return new Sound::Sample(data_path("audio/PingPongHitLow.opus"));
});

PlayMode::PlayMode() : scene(*ping_pong_scene) {
	//get pointers to leg for convenience:
	for (auto &transform : scene.transforms) {
		if (transform.name == "player_paddle") {
            player_paddle = Paddle(&transform);
        }
		else if (transform.name == "ball") {
            ball = Ball(&transform);
        }
		else if (transform.name == "cpu_paddle") cpu_paddle = &transform;
	}

	//get pointer to camera for convenience:
	if (scene.cameras.size() != 1) throw std::runtime_error("Expecting scene to have exactly one camera, but it has " + std::to_string(scene.cameras.size()));
	camera = &scene.cameras.front();

    bounce_sfx.push_back(*ping_pong_bounce_01);
    bounce_sfx.push_back(*ping_pong_bounce_02);
    bounce_sfx.push_back(*ping_pong_bounce_03);
    bounce_sfx.push_back(*ping_pong_bounce_04);

	//start music loop playing:
	// (note: position will be over-ridden in update())
//	leg_tip_loop = Sound::loop_3D(*dusty_floor_sample, 1.0f, get_leg_tip_position(), 10.0f);
}

PlayMode::~PlayMode() {
}

bool PlayMode::handle_event(SDL_Event const &evt, glm::uvec2 const &window_size) {

	if (evt.type == SDL_KEYDOWN) {
        if (evt.key.keysym.sym == SDLK_SPACE) {
            if (game_state == Start) {
                game_state = Playing;
                timing_info.previous_rally_complete = 0;
                Sound::stop_all_samples();
                on_hit();
            } else {
                on_swing_attempt();
            }
            return true;
        }
    }
    if (evt.type == SDL_MOUSEBUTTONDOWN) {
		if (SDL_GetRelativeMouseMode() == SDL_FALSE) {
			SDL_SetRelativeMouseMode(SDL_TRUE);
			return true;
		}
	} else if (evt.type == SDL_MOUSEMOTION) {
        if (game_state == Start) {
            return true;
        }
		if (SDL_GetRelativeMouseMode() == SDL_TRUE) {
			glm::vec2 motion = glm::vec2(
				evt.motion.xrel / float(window_size.y),
				-evt.motion.yrel / float(window_size.y)
			);
			player_paddle.transform->position += glm::vec3(motion.x, 0, motion.y);
			return true;
		}
	}

	return false;
}

void PlayMode::update(float elapsed) {
    if(game_state == Start) {
        return;
    }

    time_elapsed += elapsed;

    { //update ball position
        float time_since_hit = time_elapsed - timing_info.previous_rally_complete;
        float rally_length = timing_info.current_rally_complete - timing_info.previous_rally_complete;
        ball.update( time_since_hit / rally_length );
        if(!timing_info.hasBounced && time_elapsed >= timing_info.previous_rally_complete + timing_info.rally_time / 2) {
            on_bounce();
            timing_info.hasBounced = true;
        }
    }

    //position checks
    //cpu hit
    if(!player_next_to_hit && ball.transform->position.y > Constants::TABLE_LENGTH_COORDS.y) {
        on_cpu_hit();
    }
    //check for miss
    if(ball.transform->position.y < Constants::TABLE_LENGTH_COORDS.x - 4.0f) {
        reset();
    }

	{ //update listener to camera position:
		glm::mat4x3 frame = camera->transform->make_local_to_parent();
		glm::vec3 frame_right = frame[0];
		glm::vec3 frame_at = frame[3];
		Sound::listener.set_position_right(frame_at, frame_right, 1.0f / 60.0f);
	}

	//reset button press counters:
//	left.downs = 0;
//	right.downs = 0;
//	up.downs = 0;
//	down.downs = 0;
}

void PlayMode::draw(glm::uvec2 const &drawable_size) {
	//update camera aspect ratio for drawable:
	camera->aspect = float(drawable_size.x) / float(drawable_size.y);

	//set up light type and position for lit_color_texture_program:
	// TODO: consider using the Light(s) in the scene to do this
	glUseProgram(lit_color_texture_program->program);
	glUniform1i(lit_color_texture_program->LIGHT_TYPE_int, 1);
	glUniform3fv(lit_color_texture_program->LIGHT_DIRECTION_vec3, 1, glm::value_ptr(glm::vec3(0.0f, 0.0f,-1.0f)));
	glUniform3fv(lit_color_texture_program->LIGHT_ENERGY_vec3, 1, glm::value_ptr(glm::vec3(1.0f, 1.0f, 0.95f)));
	glUseProgram(0);

	glClearColor(0.5f, 0.5f, 0.5f, 1.0f);
	glClearDepth(1.0f); //1.0 is actually the default value to clear the depth buffer to, but FYI you can change it.
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS); //this is the default depth comparison function, but FYI you can change it.

	scene.draw(*camera);

	{ //use DrawLines to overlay some text:
		glDisable(GL_DEPTH_TEST);
		float aspect = float(drawable_size.x) / float(drawable_size.y);
		DrawLines lines(glm::mat4(
			1.0f / aspect, 0.0f, 0.0f, 0.0f,
			0.0f, 1.0f, 0.0f, 0.0f,
			0.0f, 0.0f, 1.0f, 0.0f,
			0.0f, 0.0f, 0.0f, 1.0f
		));

		constexpr float H = 0.09f;
		lines.draw_text("Score: " + std::to_string(score),
			glm::vec3(-aspect + 0.1f * H, -1.0 + 0.1f * H, 0.0),
			glm::vec3(H, 0.0f, 0.0f), glm::vec3(0.0f, H, 0.0f),
			glm::u8vec4(0x00, 0x00, 0x00, 0x00));
		float ofs = 2.0f / drawable_size.y;
		lines.draw_text("Score: " + std::to_string(score),
			glm::vec3(-aspect + 0.1f * H + ofs, -1.0 + 0.1f * H + ofs, 0.0),
			glm::vec3(H, 0.0f, 0.0f), glm::vec3(0.0f, H, 0.0f),
			glm::u8vec4(0xff, 0xff, 0xff, 0x00));

        constexpr float H2 = 0.05f;
        lines.draw_text("High Score: " + std::to_string(high_score),
                        glm::vec3(-aspect + H + 0.2 * H2, -1.0 + H + 0.2 * H2, 0.0),
                        glm::vec3(H2, 0.0f, 0.0f), glm::vec3(0.0f, H2, 0.0f),
                        glm::u8vec4(0x00, 0x00, 0x00, 0x00));
        ofs = 2.0f / drawable_size.y;
        lines.draw_text("High Score: " + std::to_string(high_score),
                        glm::vec3(-aspect + H + 0.2 * H2 + ofs, -1.0 + H + 0.2 * H2 + ofs, 0.0),
                        glm::vec3(H2, 0.0f, 0.0f), glm::vec3(0.0f, H2, 0.0f),
                        glm::u8vec4(0xff, 0xff, 0xff, 0x00));
	}
	GL_ERRORS();
}

void PlayMode::on_swing_attempt() {
    //TODO: check collision, pass in ball transform
//    if(player_paddle.can_swing()) {//check cooldown
        std::cout << "swing: ";
        std::cout << player_paddle.check_ball_collision(ball.transform) << std::endl;
        player_paddle.swing();
        if(player_next_to_hit && player_paddle.check_ball_collision(ball.transform)) {
            on_hit();
        }
//    }
}

void PlayMode::on_hit() {
//    std::cout << "hit!\n";
    //TODO: play sound
    if(score == 0) {
        Sound::play(*ping_pong_low_big);
    } else {
        Sound::play(*ping_pong_low);
    }

    player_next_to_hit = false;
    //generate new timing_info and rally locations
     ball.gen_rally_locations(ball.current_rally_locations.end);

    //update score
    score++;
    if(score > high_score) {
        high_score = score;
    }
    //new timing
    generate_timing();
}

void PlayMode::on_bounce() {
//    std::cout << "bounce!\n";

    int rand = std::rand() % 4;
    Sound::play(bounce_sfx.at(rand));
}

void PlayMode::on_cpu_hit() {
//    std::cout << "cpu hit!\n";
    //start music
    if(score == 1) {
        music = Sound::loop(*ping_pong_loop);
    }

    //play sound
    if(score % 2 == 1) {
        Sound::play(*ping_pong_hi_big);
    } else {
        Sound::play(*ping_pong_hi);
    }

    player_next_to_hit = true;
    ball.gen_rally_locations(ball.current_rally_locations.end);
    generate_timing();
}

void PlayMode::reset() {
    std::cout << "reset!\n";
    score = 0;
    time_elapsed = 0;
    game_state = Start;

    struct TimingInfo defTime;
    timing_info = defTime;
    struct PatternInfo defPat;
    pattern = defPat;
    Ball::RallyLocations new_locations;
    ball.current_rally_locations = new_locations;
    ball.transform->position = glm::vec3(0, 0, 1.0f);

    lose_sfx = Sound::play(*ping_pong_drop);
    music->stop();

    player_next_to_hit = true;
}

void PlayMode::generate_timing() {
    if(score >= pattern.score_to_switch_pattern) {
        generate_pattern();
    }

    //generate new timing info
    struct TimingInfo new_timing{
            pattern.pattern_rally_time,
            timing_info.current_rally_complete + new_timing.rally_time,
            timing_info.current_rally_complete,
    };
    timing_info = new_timing;

//    std::cout <<"new timing: \n" << new_timing.previous_rally_complete << "\n" << new_timing.rally_time << "\n" << new_timing.current_rally_complete << "\n";
}

void PlayMode::generate_pattern() {
//    std::cout << "new patterm!\n";
    //TODO: make different patterns
    PatternInfo new_pattern;
    new_pattern.score_to_switch_pattern = score + 8;
    new_pattern.pattern_rally_time = Constants::QUARTER_NOTE_TIME;
    pattern = new_pattern;
}
