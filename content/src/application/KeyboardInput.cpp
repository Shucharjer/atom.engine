#include "application/KeyboardInput.hpp"
#include <GLFW/glfw3.h>

using namespace atom::engine::application;

KeyboardInput::KeyboardInput() {}

void KeyboardInput::Callback(GLFWwindow* pWindow, int key, int scancode, int action, int mods) {
    auto& inst = instance();

    switch (action) {
    case GLFW_PRESS:
        inst.m_IsPressing[key] = true;
        if (inst.m_PressCallbacks[key])
            inst.m_PressCallbacks[key](pWindow);
        break;
    case GLFW_RELEASE:
        inst.m_IsPressing[key] = false;
        if (inst.m_ReleaseCallbacks[key])
            inst.m_ReleaseCallbacks[key](pWindow);
        break;
    default:
        break;
    }
}

void KeyboardInput::setPressCallback(int key, void (*callback)(GLFWwindow*)) {
    m_PressCallbacks[key] = callback;
}

void KeyboardInput::setReleaseCallback(int key, void (*callback)(GLFWwindow*)) {
    m_ReleaseCallbacks[key] = callback;
}

bool KeyboardInput::isPressing(int key) noexcept { return m_IsPressing[key]; }
