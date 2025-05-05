#pragma once

#include "pchs/math.hpp"
namespace atom::engine::systems::render {

struct Light {};

struct DirectionalLight : public Light {
    math::Vector3 direction;
    math::Vector3 color;
    float intensity;
};

struct PointLight : public Light {
    math::Vector3 position;
    math::Vector3 color;
    float intensity;

    float constant;
    float linear;
    float quadratic;
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
