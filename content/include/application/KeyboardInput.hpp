#pragma once
#include <glad/glad.h>
//
#include <unordered_map>
#include <GLFW/glfw3.h>
#include <auxiliary/singleton.hpp>

namespace atom::engine::application {

class KeyboardInput : public utils::singleton<KeyboardInput> {
    friend class utils::singleton<KeyboardInput>;

    KeyboardInput();

public:
    static void Callback(GLFWwindow* pWindow, int key, int scancode, int action, int mods);

    void setPressCallback(int key, void(*callback)());
    void setReleaseCallback(int key, void(*callback)());
private:
    std::unordered_map<int, void(*)()> m_PressCallbacks;
    std::unordered_map<int, void(*)()> m_ReleaseCallbacks;
};

} // namespace atom::engine::application
