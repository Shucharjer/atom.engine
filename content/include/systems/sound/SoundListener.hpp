#pragma once
#include <utility>
#include "pchs/math.hpp"

namespace atom::engine::systems::sound {

class SoundListener {
public:
    SoundListener();

    void setPosition(const float* position) const noexcept;
    void setPosition(const math::Vector3& position) const noexcept;
    void setPosition(const float x, const float y, const float z) const noexcept;

    void setOrientation(const float* orientation) const noexcept;
    void setOrientation(const math::Vector3& forward, const math::Vector3& up) const noexcept;
    void setOrientation(
        const float forwardX,
        const float forwardY,
        const float forwardZ,
        const float upX,
        const float upY,
        const float upZ
    ) const noexcept;

    [[nodiscard]] std::pair<math::Vector3, math::Vector3> getOrientation() const noexcept;

private:
};

} // namespace atom::engine::systems::sound
