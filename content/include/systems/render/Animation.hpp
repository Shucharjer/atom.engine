#pragma once
#include "pchs/math.hpp"

namespace atom::engine::systems::render {

struct KeyPosition {
    math::Vector3 position;
    float timeStamp;
};

struct KeyRotation {
    math::Quaternion orientation;
    float timeStamp;
};

struct KeyScale {
    math::Vector3 scale;
    float timeStamp;
};

class Animation {};

} // namespace atom::engine::systems::render
