//
// Created by William Ozeas on 9/15/22.
//

#include "Paddle.hpp"
Paddle::Paddle() {}

Paddle::Paddle(Scene::Transform *_transform) {
    transform = _transform;
}

Paddle::Paddle(Paddle &paddle) {
    transform = paddle.transform;
}

void Paddle::swing() {
    swing_cd = 0;
    //TODO: swing animation
}

bool Paddle::can_swing() {
    return swing_cd >= Constants::SWING_CD_TIME;
}

void Paddle::update(float elapsed) {
    swing_cd += elapsed;
}

bool Paddle::check_ball_collision(Scene::Transform *ball) {
    glm::vec3 ball_corner_one = ball->position + Constants::BALL_AABB_DIMENSIONS / 2.0f; //position
    glm::vec3 ball_corner_two  = ball->position - Constants::BALL_AABB_DIMENSIONS / 2.0f;

    glm::vec3 ball_corner_greater(std::max(ball_corner_one.x, ball_corner_two.x), std::max(ball_corner_one.y, ball_corner_two.y),
                                  std::max(ball_corner_one.z, ball_corner_two.z));
    glm::vec3 ball_corner_lesser(std::min(ball_corner_one.x, ball_corner_two.x), std::min(ball_corner_one.y, ball_corner_two.y),
                                 std::min(ball_corner_one.z, ball_corner_two.z));

    glm::vec3 bb_corner_one = transform->position + Constants::PADDLE_AABB_DIMENSIONS / 2.0f + Constants::PADDLE_AABB_OFFSET / 2.0f;
    glm::vec3 bb_corner_two  = transform->position - Constants::PADDLE_AABB_DIMENSIONS / 2.0f + Constants::PADDLE_AABB_OFFSET;

    glm::vec3 bb_corner_greater(std::max(bb_corner_one.x, bb_corner_two.x), std::max(bb_corner_one.y, bb_corner_two.y),
                                std::max(bb_corner_one.z, bb_corner_two.z));
    glm::vec3 bb_corner_lesser(std::min(bb_corner_one.x, bb_corner_two.x), std::min(bb_corner_one.y, bb_corner_two.y),
                               std::min(bb_corner_one.z, bb_corner_two.z));

    bool x_coll = false;
    bool y_coll = false;
    bool z_coll = false;
    if (ball_corner_lesser.x <= bb_corner_greater.x && ball_corner_greater.x >= bb_corner_lesser.x) {
        x_coll = true;
    }
    if (ball_corner_lesser.y <= bb_corner_greater.y && ball_corner_greater.y >= bb_corner_lesser.y) {
        y_coll = true;
    }
    if (ball_corner_lesser.z <= bb_corner_greater.z && ball_corner_greater.z >= bb_corner_lesser.z) {
        z_coll = true;
    }

    return x_coll && y_coll && z_coll;
}
