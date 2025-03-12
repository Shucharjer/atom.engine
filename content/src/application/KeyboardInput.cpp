#include "application/KeyboardInput.hpp"
#include <GLFW/glfw3.h>

using namespace atom::engine::application;

KeyboardInput::KeyboardInput() {
    
}

void KeyboardInput::Callback(GLFWwindow* pWindow, int key, int scancode, int action, int mods) {
    auto& inst = instance();

    switch (action) {
    case GLFW_PRESS:
        if (inst.m_PressCallbacks[key]) inst.m_PressCallbacks[key]();
        break;
    case GLFW_RELEASE:
        if (inst.m_ReleaseCallbacks[key]) inst.m_ReleaseCallbacks[key]();
        break;
    default:
        break;
    }
}

void KeyboardInput::setPressCallback(int key, void(*callback)()) {
    m_PressCallbacks[key] = callback;
}

void KeyboardInput::setReleaseCallback(int key, void(*callback)()) {
    m_ReleaseCallbacks[key] = callback;
}
