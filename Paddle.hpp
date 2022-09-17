//
// Created by William Ozeas on 9/15/22.
//

#ifndef INC_15_466_F22_GAME3_PADDLE_HPP
#define INC_15_466_F22_GAME3_PADDLE_HPP

#include "Scene.hpp"
#include "Constants.hpp"


class Paddle {
private:
    float swing_cd;

public:
    Paddle();
    explicit Paddle(Scene::Transform *_transform);
    Paddle(Paddle &paddle);

    Scene::Transform *transform = nullptr;

    bool can_swing();
    bool check_ball_collision(Scene::Transform *ball);

    void swing();
    void update(float elapsed);

};

#endif //INC_15_466_F22_GAME3_PADDLE_HPP
