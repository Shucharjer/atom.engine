#pragma once
#include "pch.hpp"

namespace atom::engine::systems::render {

class Camera {
    math::Vector3 position;
    math::Quaternion rotation;
};

} // namespace atom::engine::systems::render
