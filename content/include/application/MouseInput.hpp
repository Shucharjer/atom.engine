#pragma once
#include <glad/glad.h>
//
#include <unordered_map>
#include <GLFW/glfw3.h>
#include <auxiliary/singleton.hpp>

namespace atom::engine::application {

class MouseInput : public utils::singleton<MouseInput> {
    friend class utils::singleton<MouseInput>;

    MouseInput() = default;

public:
    static void Callback();

private:
    void(*m_PressCallback)();
    void(*m_ReleaseCallback)();
};

} // namespace atom::engine::application
