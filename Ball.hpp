//
// Created by William Ozeas on 9/15/22.
//

#ifndef INC_15_466_F22_GAME3_BALL_HPP
#define INC_15_466_F22_GAME3_BALL_HPP

#include "Scene.hpp"
#include "Constants.hpp"

class Ball {
private:
    glm::vec3 original_pos;

public:
    struct RallyLocations {
        glm::vec3 start = glm::vec3(0.0f);
        glm::vec3 bounce = glm::vec3(0.0f);
        glm::vec3 end = glm::vec3(0.0f, -5.0f, 1.0f);;
    } current_rally_locations;

    Ball();
    Ball(Scene::Transform *_transform);

    void update(float percent_elapsed);

    void gen_rally_locations(glm::vec3 start);
    void reset();

    Scene::Transform *transform;

};

#endif //INC_15_466_F22_GAME3_BALL_HPP
