#pragma once
#include "pchs/math.hpp"

namespace atom::engine::systems::render {

struct Transform {
    math::Vector3 position;
    math::Quaternion rotation;
    math::Vector3 scaling;
};

} // namespace atom::engine::systems::render
