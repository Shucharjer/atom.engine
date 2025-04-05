#pragma once
#include <core/langdef.hpp>
#include <glm/ext/matrix_clip_space.hpp>
#include <glm/trigonometric.hpp>
#include "pchs/math.hpp"

namespace atom::engine::systems::render {

constexpr float kDefaultFov       = 45.F;
constexpr float kDefaultAspect    = 16.0F / 9.0F;
constexpr float kClassicAspect    = 4.0F / 3.0F;
constexpr float kDefaultNearPlane = 0.1F;
constexpr float kDefaultFarPlane  = 1000.F;

struct alignas(num_sixteen) Camera {
    math::Vector3 position;
    math::Quaternion orientation;
    math::Vector3 forward;
    math::Vector3 up;
    float fov;
    float aspect;
    float nearPlane;
    float farPlane;

    Camera(
        const math::Vector3& pos    = math::Vector3(0.0F),
        const math::Quaternion& ori = math::Quaternion(),
        const math::Vector3& fwd    = math::Vector3(1.0F, 0.0F, 0.0F),
        const math::Vector3& up     = math::Vector3(0.0F, 1.0F, 0.0F),
        const float fov             = kDefaultFov,
        const float aspect          = kDefaultAspect,
        const float nearPlane       = kDefaultNearPlane,
        const float farPlane        = kDefaultFarPlane
    ) noexcept
        : position(pos), orientation(ori), forward(fwd), up(up), fov(fov), aspect(aspect),
          nearPlane(nearPlane), farPlane(farPlane) {}

    Camera(const Camera& that) noexcept = default;

    Camera(Camera&& that) noexcept = default;

    Camera& operator=(const Camera& that) noexcept = default;

    Camera& operator=(Camera&& that) noexcept = default;

    ~Camera() = default;

    [[nodiscard]] math::Mat4 view() const noexcept {
        return glm::translate(glm::mat4_cast(orientation), position);
    }

    [[nodiscard]] math::Mat4 proj() const noexcept {
        return glm::perspective(glm::radians(fov), aspect, nearPlane, farPlane);
    }

    void rotate(const float angle, const glm::vec3 axis) noexcept {
        math::Quaternion deltaQuat = glm::angleAxis(glm::radians(angle), axis);
        orientation                = glm::normalize(deltaQuat * orientation);
    }
};

} // namespace atom::engine::systems::render
