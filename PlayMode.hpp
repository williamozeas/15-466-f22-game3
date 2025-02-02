#include "Mode.hpp"

#include "Scene.hpp"
#include "Sound.hpp"
#include "Paddle.hpp"
#include "Ball.hpp"
#include "Load.hpp"

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/glm.hpp>
#include "glm/ext.hpp"


#include <vector>
#include <deque>

struct PlayMode : Mode {
	PlayMode();
	virtual ~PlayMode();

	//functions called by main loop:
	virtual bool handle_event(SDL_Event const &, glm::uvec2 const &window_size) override;
	virtual void update(float elapsed) override;
	virtual void draw(glm::uvec2 const &drawable_size) override;
    virtual void reset();

    void on_swing_attempt();
    void on_bounce();
    void on_cpu_hit();
    void on_hit();
    void generate_pattern();
    void generate_timing();

	//----- game state -----
    enum GameState {
        Start,
        Playing
    };
    GameState game_state = Start;

    bool player_next_to_hit = true;

    struct TimingInfo {
        float rally_time = Constants::QUARTER_NOTE_TIME;
        float current_rally_complete = 0;
        float previous_rally_complete = 0;
        bool hasBounced = false;
    };
    TimingInfo timing_info;

    struct PatternInfo {
        uint32_t score_to_switch_pattern = 5;
        float pattern_rally_time = Constants::QUARTER_NOTE_TIME;
    } pattern;

	//input tracking:
//	struct Button {
//		uint8_t downs = 0;
//		uint8_t pressed = 0;
//	} left, right, down, up;

    float time_elapsed = 0;
    uint32_t score = 0;
    uint32_t high_score = 0;



	//local copy of the game scene (so code can change it during gameplay):
	Scene scene;

	//hexapod leg to wobble:
	Paddle player_paddle;
	Ball ball;
    Scene::Transform *cpu_paddle = nullptr;

//

	//music:
	std::shared_ptr< Sound::PlayingSample > music;
    std::shared_ptr< Sound::PlayingSample > lose_sfx;

    std::vector< Sound::Sample > bounce_sfx;
	
	//camera:
	Scene::Camera *camera = nullptr;

};
