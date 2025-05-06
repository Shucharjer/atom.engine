#include "MouseCallback.hpp"
#include <GLFW/glfw3.h>
#include "Local.hpp"
#include "pchs/math.hpp"

static bool init = false;
static double lastX;
static double lastY;

using namespace atom::engine::math;

void MouseCallback(GLFWwindow* window) {
    double x{}, y{};
    glfwGetCursorPos(window, &x, &y);

    if (!gEnableMouseInput) {
        return;
    }

    if (!init) [[unlikely]] {
        lastX = x;
        lastY = y;
        init  = true;
        return;
    }

    auto offsetX = x - lastX;
    auto offsetY = y - lastY;

    offsetX *= gLocalSensitivity;
    offsetY *= gLocalSensitivity;

    gCamera->rotate(-offsetY, Vector3(1.0f, 0.0f, 0.0f));
    gCamera->rotate(-offsetX, Vector3(0.0f, 1.0f, 0.0f));

    lastX = x;
    lastY = y;
}
