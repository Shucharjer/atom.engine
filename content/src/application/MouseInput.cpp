#include "application/MouseInput.hpp"

namespace atom::engine::application {

void MouseInput::Callback(GLFWwindow* window, double x, double y) {
    auto& instance = MouseInput::instance();
    instance.m_Callback(window, x, y);
}

} // namespace atom::engine::application
