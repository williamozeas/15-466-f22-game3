//
// Created by William Ozeas on 9/15/22.
//
#define GLM_ENABLE_EXPERIMENTAL
#include <iostream>
#include "Ball.hpp"
#include "glm/ext.hpp"

Ball::Ball() {
};

Ball::Ball(Scene::Transform *_transform) {
    transform = _transform;
    original_pos = _transform->position;
}

void Ball::update(float percent_elapsed) {
    glm::vec3 new_loc;
    if(percent_elapsed < 0.5f) {
        new_loc = Constants::lerp(current_rally_locations.start,
                                              current_rally_locations.bounce, percent_elapsed * 2);
    } else {
        new_loc = Constants::lerp(current_rally_locations.bounce,
                                              current_rally_locations.end, (percent_elapsed - 0.5f) * 2);
    }
    transform->position = new_loc;
}

void Ball::gen_rally_locations(glm::vec3 start) {
    current_rally_locations.start = start;

    {
        //random number generation quick code from https://stackoverflow.com/questions/686353/random-float-number-generation
        float x = Constants::TABLE_WIDTH_COORDS.x + static_cast <float> (rand()) /
                                                    (static_cast <float> (RAND_MAX / (Constants::TABLE_WIDTH_COORDS.y -
                                                                                      Constants::TABLE_WIDTH_COORDS.x)));
        float y;
        float center = (Constants::TABLE_LENGTH_COORDS.x + Constants::TABLE_LENGTH_COORDS.y)/2;
        if(current_rally_locations.start.y > center) {
            y = Constants::TABLE_LENGTH_COORDS.x;
        } else {
            y = Constants::TABLE_LENGTH_COORDS.y;
        }
        float z = Constants::BALL_BOUNCE_HEIGHT.x + static_cast <float> (rand()) /
                                                    (static_cast <float> (RAND_MAX / (Constants::BALL_BOUNCE_HEIGHT.y -
                                                                                      Constants::BALL_BOUNCE_HEIGHT.x)));
        current_rally_locations.end = glm::vec3(x, y, z);
    }

    glm::vec3 middle = Constants::lerp(current_rally_locations.start, current_rally_locations.end, 0.5f);
    current_rally_locations.bounce = glm::vec3(middle.x, middle.y, 0.0f);

//    std::cout << "new locations: \n " << glm::to_string(current_rally_locations.start) << "\n" << glm::to_string(current_rally_locations.bounce) << "\n" << glm::to_string(current_rally_locations.end) << "\n";
}

void Ball::reset() {

}




