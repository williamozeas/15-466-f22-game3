//
// Created by William Ozeas on 9/16/22.
//

#ifndef INC_15_466_F22_GAME3_CONSTANTS_HPP
#define INC_15_466_F22_GAME3_CONSTANTS_HPP

#include <glm/glm.hpp>

struct Constants {
    constexpr static glm::vec2 TABLE_WIDTH_COORDS = glm::vec2(-2.0f, 2.0f);
    constexpr static glm::vec2 TABLE_LENGTH_COORDS = glm::vec2(-5, 5);
    constexpr static glm::vec2 BALL_BOUNCE_HEIGHT = glm::vec2(0.5f, 2.0f);

    constexpr static glm::vec3 PADDLE_AABB_DIMENSIONS = glm::vec3(2.0f, 2.0f, 2.0f);
    constexpr static glm::vec3 PADDLE_AABB_OFFSET = glm::vec3(0, 1.0f, 0);
    constexpr static glm::vec3 BALL_AABB_DIMENSIONS = glm::vec3(0.15f, 0.15f, 0.15f);

    constexpr static float SWING_CD_TIME = 0.4f;
    constexpr static float BPM = 120;
    constexpr static float QUARTER_NOTE_TIME = 60 / BPM;

    constexpr static float MOUSE_SENSITIVITY = 1.4f;

    //lerp copied from https://graphicscompendium.com/opengl/22-interpolation
    static glm::vec3 lerp(glm::vec3 x, glm::vec3 y, float t) {
        return x * (1.f - t) + y * t;
    }
};

#endif //INC_15_466_F22_GAME3_CONSTANTS_HPP
