#include "MouseCallback.hpp"
#include <GLFW/glfw3.h>
#include <glm/ext/quaternion_geometric.hpp>
#include <glm/ext/quaternion_trigonometric.hpp>
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

    float offsetX = lastX - x;
    float offsetY = y - lastY;

    offsetX *= gLocalSensitivity;
    offsetY *= gLocalSensitivity;

    gCamera->rotate(offsetY, gCamera->left);
    gCamera->rotate(offsetX, gCamera->up);

    lastX = x;
    lastY = y;
}
