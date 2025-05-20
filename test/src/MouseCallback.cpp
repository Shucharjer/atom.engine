#include "MouseCallback.hpp"
#include <format>
#include <GLFW/glfw3.h>
#include <easylogging++.h>
#include <glm/ext/quaternion_geometric.hpp>
#include <glm/ext/quaternion_trigonometric.hpp>
#include "Local.hpp"
#include "pchs/math.hpp"

static bool init = false;
static double lastX;
static double lastY;
static atom::engine::math::Vector3 currentAngle = { 0.0F, 0.0F, 0.0F };

using namespace atom::engine::math;

void MouseCallback(GLFWwindow* window) {
    double x{}, y{};
    glfwGetCursorPos(window, &x, &y);

    if (!gEnableMouseInput) {
        init = false;
        return;
    }

    if (!init) [[unlikely]] {
        lastX = x;
        lastY = y;
        init  = true;
        return;
    }

    float offsetX = lastX - x;
    float offsetY = y - lastY;

    offsetX *= gLocalSensitivity;
    offsetY *= gLocalSensitivity;
    currentAngle.x -= offsetY * gDeltaTime * gLocalSensitivity;
    currentAngle.y += offsetX * gDeltaTime * gLocalSensitivity;
    currentAngle.x = std::clamp(currentAngle.x, -89.0F, 89.0F);
    currentAngle.y = std::fmod(currentAngle.y, 360.0F);
    gCamera->rotationEulerAngle(currentAngle);

    lastX = x;
    lastY = y;

    auto eulerAngles = gCamera->eulerAngles();
    LOG(INFO) << std::format(
        "current eular angles: {}, {}, {}", eulerAngles.x, eulerAngles.y, eulerAngles.z
    );
}
