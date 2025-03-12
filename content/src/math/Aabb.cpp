#include "math/Aabb.hpp"
#include <core/langdef.hpp>
#include "pch.hpp"

namespace atom::engine::math {

Vector3 Aabb::getCenter() const noexcept {
    if constexpr (requires(Vector3 a, Vector3 b) { a + b; } && requires(Vector3 v, float f) { v* f; }) {
        return (m_Min + m_Max) * num_f_half;
    }
    else {
        return Vector3(
            (m_Min.x + m_Max.x) * num_f_half,
            (m_Min.y + m_Max.y) * num_f_half,
            (m_Min.z + m_Max.z) * num_f_half
        );
    }
}

} // namespace atom::engine::math
