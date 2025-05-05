#pragma once
#include "pchs/math.hpp"

namespace atom::engine::systems::render {

struct Transform {
    math::Vector3 position;
    math::Quaternion rotation{ 1.0F, 0.0F, 0.0F, 0.0F };
    math::Vector3 scaling{ 1.0F, 1.0F, 1.0F };

    [[nodiscard]] math::Mat4 toMatrix() const noexcept {
        return glm::translate(math::Mat4(1.0F), position) * glm::toMat4(rotation) *
               glm::scale(math::Mat4(1.0F), scaling);
    }
};

} // namespace atom::engine::systems::render
