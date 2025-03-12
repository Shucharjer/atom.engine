#pragma once
#include "pch.hpp"

namespace atom::engine::systems::render {

struct Vertex {
    math::Vector3 position;
    float _padding;
    math::Vector3 normal;
    float _padding_;
    math::Vector3 texCoords;
};

}