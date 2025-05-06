#include "MouseCallback.hpp"
#include <iostream>
#include <GLFW/glfw3.h>
#include "Local.hpp"
#include "pchs/math.hpp"

bool init = false;
double lastX;
double lastY;

using namespace atom::engine::math;

void MouseCallback(GLFWwindow* window) {
    std::cout << "mouse callback\n";
    double x{}, y{};
    glfwGetCursorPos(window, &x, &y);

    if (!init) {
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
