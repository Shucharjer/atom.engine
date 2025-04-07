#pragma once
#include <filesystem>
#include <glad/glad.h>
//
#include <GLFW/glfw3.h>
#include <imgui.h>
#include <imgui_impl_opengl3.h>
#include <asset.hpp>
#include <core/langdef.hpp>
#include <reflection.hpp>
#include <world.hpp>
#include "application/Panel.hpp"

namespace atom::engine::application {

const bool kDefaultFullscreen  = false;
const int kDefaultVsync        = 0;
const int kDefaultWidth        = 1280;
const int kDefaultHeight       = 960;
const int kFpsMax              = 144;
const char* const kDefaultName = "Untitled Window";

#if defined(_WIN32) || defined(_WIN64)
constexpr const char* kFont = R"(C:\Windows\Fonts\STSONG.TTF)";
#elif defined(__linux) || defined(__linux__)
constexpr const char* kFont = "/usr/share/fonts/.TTF";
#else
    #error "Unknown system"
#endif

struct FontConfig {
    std::string file = kFont;
    float sizePixels = num_sixteen;
};

struct WindowConfig {
    bool fullscreen  = kDefaultFullscreen;
    int vsync        = kDefaultVsync;
    int width        = kDefaultWidth;
    int height       = kDefaultHeight;
    int fpsMax       = kFpsMax;
    std::string name = kDefaultName;
    FontConfig fontConfig{};
};

auto LoadWindowConfig(const std::filesystem::path& path) -> WindowConfig;
void SaveWindowConfig(const std::filesystem::path& path, const WindowConfig& config);

class Window {
public:
    explicit Window(const WindowConfig& config = WindowConfig{}, GLFWwindow* shareWindow = nullptr);
    explicit Window(WindowConfig&& config, GLFWwindow* shareWindow = nullptr);

    Window(const Window&)            = delete;
    Window(Window&&)                 = delete;
    Window& operator=(const Window&) = delete;
    Window& operator=(Window&&)      = delete;
    virtual ~Window();

    WindowConfig& config();

    /**
     * @brief Initlize the window.
     *
     * @return int When initialize successfully, it will return 0.
     */
    int init();

    /**
     * @brief Run the stuffs in the window.
     *
     * Each world would startup when calling this function, then update until close.
     */
    void run();

    /**
     * @brief Terminate the window.
     *
     */
    void terminate();

protected:
    virtual int onInit() { return 0; }
    virtual void onConfigPanelsLayout() {}
    virtual void afterConfigPanelsLayout() {}
    virtual void onTerminate() {}

    /**
     * @brief Emplace a panel.
     *
     */
    void emplace(const std::string& name, Panel* panel) { m_Panels[name] = panel; }

    /**
     * @brief Weather contains a panel.
     *
     */
    [[nodiscard]] bool contains(const std::string& name) const noexcept;

    /**
     * @brief Erase a panel
     *
     */
    void erase(std::string_view name) noexcept;

    std::shared_ptr<::atom::ecs::world> createWorld(const std::string& name);
    atom::map<std::string, std::shared_ptr<::atom::ecs::world>>& worlds();

private:
    void startup();
    void update(float deltaTime);
    void shutdown();

    bool m_Terminate = false;

    atom::map<std::string, std::shared_ptr<::atom::ecs::world>> m_Worlds;
    atom::map<std::string, Panel*> m_Panels;
    void (*m_RenderCallback)() = nullptr;

    WindowConfig m_WindowConfig;
    GLFWwindow* m_pWindow = nullptr;
    GLFWwindow* m_pShareWindow;
};

void SetupWindow(Window& window);

} // namespace atom::engine::application
