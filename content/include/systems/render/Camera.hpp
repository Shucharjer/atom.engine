#pragma once
#include <easylogging++.h>
#include <core/langdef.hpp>
#include <glm/ext/matrix_clip_space.hpp>
#include <glm/ext/quaternion_geometric.hpp>
#include <glm/geometric.hpp>
#include <glm/trigonometric.hpp>
#include "pchs/math.hpp"

namespace atom::engine::systems::render {

constexpr float kDefaultFov       = 30.F;
constexpr float kDefaultAspect    = 16.0F / 9.0F;
constexpr float kClassicAspect    = 4.0F / 3.0F;
constexpr float kDefaultNearPlane = 0.1F;
constexpr float kDefaultFarPlane  = 1000.F;

struct alignas(magic_16) Camera {
    math::Vector3 position{};
    math::Quaternion orientation{ 1.0F, 0.0F, 0.0F, 0.0F };
    math::Vector3 forward = math::Vector3(0.0F, 0.0F, -1.0F);
    math::Vector3 up      = math::Vector3(0.0F, 1.0F, 0.0F);
    math::Vector3 left    = math::Vector3(-1.0F, 0.0F, 0.0F);
    float fov             = kDefaultFov;
    float aspect          = kDefaultAspect;
    float nearPlane       = kDefaultNearPlane;
    float farPlane        = kDefaultFarPlane;

    [[nodiscard]] math::Mat4 view() const noexcept {
        // return glm::lookAt(position, position + forward, up);
        return glm::translate(glm::mat4_cast(glm::inverse(orientation)), -position);
    }

    [[nodiscard]] math::Mat4 proj() const noexcept {
        return glm::perspective(glm::radians(fov), aspect, nearPlane, farPlane);
    }

    [[nodiscard]] math::Vector3 eulerAngles() const noexcept {
        return glm::degrees(glm::eulerAngles(orientation));
    }

    void rotate(const float angle, const glm::vec3 axis) noexcept {
        math::Quaternion deltaQuat = glm::angleAxis(glm::radians(angle), axis);
        orientation                = glm::normalize(orientation * deltaQuat);
        updateDirectionVector();
    }
    void rotationEulerAngle(math::Vector3 eluerAngle) noexcept {
        orientation = glm::quat(glm::vec3(
            glm::radians(eluerAngle.x), glm::radians(eluerAngle.y), glm::radians(eluerAngle.z)
        ));
        updateDirectionVector();
    }
    void updateDirectionVector() {
        // sync direction vectors with the new orientation
        forward = orientation * math::Vector3(0.0f, 0.0f, -1.0f);
        up      = orientation * math::Vector3(0.0F, 1.0F, 0.0F);
        left    = glm::normalize(-glm::cross(forward, up));
    }
};

} // namespace atom::engine::systems::render
