#pragma once
#include <glad/glad.h>
//
#include <array>
#include <GLFW/glfw3.h>
#include <auxiliary/singleton.hpp>
#include <signal/delegate.hpp>

namespace atom::engine::application {
const uint8_t kMaxMouseButtonCount = 8;

class MouseInput : public utils::singleton<MouseInput> {
    friend class utils::singleton<MouseInput>;

    MouseInput() = default;

public:
    static void Callback(GLFWwindow* window, double x, double y);
    static void ButtonCallback(GLFWwindow* window, int button, int action, int mods);

    template <auto Candidate>
    void bind() noexcept {
        m_Callback.template bind<Candidate>();
    }

    template <auto Candidate, typename Ty>
    void bind(Ty& inst) noexcept {
        m_Callback.template bind<Candidate>(inst);
    }

    /**
     * @param button Range: [GLFW_MOUSE_BUTTON_1, GLFW_MOUSE_BUTTON_8]
     */
    [[nodiscard]] bool isPressing(int button) const noexcept;

private:
    utils::delegate<void(GLFWwindow* window)> m_Callback;
    std::array<bool, kMaxMouseButtonCount> m_IsPressing{};
};

} // namespace atom::engine::application
