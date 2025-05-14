#pragma once

#include "pchs/math.hpp"
namespace atom::engine::systems::render {

struct Light {};

struct DirectionalLight : public Light {
    math::Quaternion orientation = { 1.0F, 0.0F, 0.0F, 0.0F };
    math::Vector3 direction      = { 0.0F, 0.0F, 1.0F };
    math::Vector3 color          = { 1.0F, 1.0F, 1.0F };
    float intensity              = 1.0F;

    void rotate(const float angle, const glm::vec3 axis) noexcept {
        math::Quaternion deltaQuat = glm::angleAxis(glm::radians(angle), axis);
        orientation                = glm::normalize(orientation * deltaQuat);
        direction                  = glm::rotate(orientation, math::Vector3(0.0F, 0.0F, -1.0F));
    }
    void rotationEulerAngle(math::Vector3 eluerAngle) noexcept {
        orientation = glm::quat(
            glm::vec3(
                glm::radians(eluerAngle[0]),
                glm::radians(eluerAngle[1]),
                glm::radians(eluerAngle[2]),
            )
        );
        // sync direction vectors with the new orientation
        direction = glm::rotate(orientation, math::Vector3(0.0F, 0.0F, -1.0F));
    }
};

struct PointLight : public Light {
    math::Vector3 position = { 0.0F, 0.0F, 0.0F };
    math::Vector3 color    = { 1.0F, 1.0F, 1.0F };
    float intensity        = 1.0F;

    float constant  = 1.0F;
    float linear    = 0.09F;
    float quadratic = 0.032F;
};

struct SpotLight : public Light {
    math::Vector3 position;
    math::Vector3 direction;
    math::Vector3 color;
    float intensity;

    float constant;
    float linear;
    float quadratic;

    float innerCutOff;
    float outerCutOff;
};

} // namespace atom::engine::systems::render
