#pragma once
#include "pchs/math.hpp"

namespace atom::engine::systems::render {

struct Vertex {
    math::Vector3 position;
    math::Vector3 normal;
    math::Vector2 texCoords;
};

} // namespace atom::engine::systems::render
