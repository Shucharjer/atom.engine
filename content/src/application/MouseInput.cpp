#include "application/MouseInput.hpp"

namespace atom::engine::application {

void MouseInput::Callback(GLFWwindow* window, double x, double y) {
    auto& instance = MouseInput::instance();
    instance.m_Callback(window);
}

void MouseInput::ButtonCallback(GLFWwindow* window, int button, int action, int mods) {
    auto& instance = MouseInput::instance();
    switch (action) {
    case GLFW_PRESS:
        instance.m_IsPressing.at(button) = true;
    case GLFW_RELEASE:
        instance.m_IsPressing.at(button) = false;
        break;
    default:
        break;
    }
}

bool MouseInput::isPressing(int button) const noexcept { return m_IsPressing.at(button); }

} // namespace atom::engine::application
