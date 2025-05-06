#pragma once
#include <glad/glad.h>
//
#include <unordered_map>
#include <GLFW/glfw3.h>
#include <auxiliary/singleton.hpp>
#include <signal/delegate.hpp>

namespace atom::engine::application {

class MouseInput : public utils::singleton<MouseInput> {
    friend class utils::singleton<MouseInput>;

    MouseInput() = default;

public:
    static void Callback(GLFWwindow* window, double x, double y);

    template <auto Candidate>
    void bind() noexcept {
        m_Callback.template bind<Candidate>();
    }

    template <auto Candidate, typename Ty>
    void bind(Ty& inst) noexcept {
        m_Callback.template bind<Candidate>(inst);
    }

private:
    utils::delegate<void(GLFWwindow* window)> m_Callback;
};

} // namespace atom::engine::application
