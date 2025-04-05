#pragma once
#include "pchs/math.hpp"

namespace atom::engine::math {

class Aabb {
public:
    void setMin(const Vector3& vec) noexcept;
    void setMinX(float x) noexcept;
    void setMinY(float y) noexcept;
    void setMinZ(float z) noexcept;

    void setMax(const Vector3& vec) noexcept;
    void setMaxX(float x) noexcept;
    void setMaxY(float y) noexcept;
    void setMaxZ(float z) noexcept;

    [[nodiscard]] const Vector3& getMin() const noexcept;
    Vector3& getMin() noexcept;

    [[nodiscard]] const Vector3& getMax() const noexcept;
    Vector3& getMax() noexcept;

    [[nodiscard]] Vector3 getCenter() const noexcept;
    [[nodiscard]] Vector3 getSize() const noexcept;

    [[nodiscard]] float distance(const Vector3& vec) const noexcept;

private:
    Vector3 m_Min;
    Vector3 m_Max;
};

} // namespace atom::engine::math
