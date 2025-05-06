#include "application/Window.hpp"
#include <array>
#include <filesystem>
#include <fstream>
#include <memory>
#include <GLFW/glfw3.h>
#include <easylogging++.h>
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>
#include <core/langdef.hpp>
#include <reflection.hpp>
#include "application/KeyboardInput.hpp"
#include "application/MouseInput.hpp"
#include "application/application.hpp"
#include "io/file.hpp"

namespace atom::engine::application {

auto LoadWindowConfig(const std::filesystem::path& path) -> WindowConfig {
    std::ifstream stream(path);
    WindowConfig config{};

    if (stream.is_open()) {
        nlohmann::json json;
        try {
            stream >> json;
        }
        catch (...) {
            LOG(ERROR) << "Failed to read file as json";
        }

        stream.close();

        WindowConfig temp;
        try {
            ::from_json(json, temp);
            config.fullscreen = temp.fullscreen;
            config.vsync      = temp.vsync;
            config.width      = temp.width;
            config.height     = temp.height;
            config.fpsMax     = temp.fpsMax;
            config.name       = temp.name;
            if (!temp.fontConfig.file.empty()) {
                config.fontConfig.sizePixels = temp.fontConfig.sizePixels;
                config.fontConfig.file       = temp.fontConfig.file;
            }
        }
        catch (...) {
            LOG(ERROR) << "Failed to deserialize window config.";
        }
    }
    else {
        LOG(INFO) << "WindowConfig not exist";
    }

    return config;
}

void SaveWindowConfig(const std::filesystem::path& path, const WindowConfig& config) {
    io::CheckFileFolder(path);

    std::ofstream stream(path, std::fstream::out | std::fstream::trunc);

    if (stream.is_open()) {
        nlohmann::json json;
        ::to_json(json, config);
        try {
            stream << json.dump(4);
        }
        catch (...) {
            LOG(ERROR) << "Failed to open file {" << path << "}";
        }
    }
}

Window::Window(const WindowConfig& config, GLFWwindow* shareWindow)
    : m_WindowConfig(config), m_pShareWindow(shareWindow), m_Worlds(), m_Panels() {}

Window::Window(WindowConfig&& config, GLFWwindow* shareWindow)
    : m_WindowConfig(std::move(config)), m_pShareWindow(shareWindow), m_Worlds(), m_Panels() {}

Window::~Window() { terminate(); }

WindowConfig& Window::config() { return m_WindowConfig; }

static inline void GlfwErrorCallback(int errorCode, const char* description) {
    LOG(ERROR) << "Glfw error code: " << errorCode << ", " << description;
}

static inline void GlfwFramebufferCallback(GLFWwindow* pWindow, int width, int height) {
    glViewport(0, 0, width, height);
}

const int kGlfwContextVersionMajor = 4;
const int kGlfwContextVersionMinor = 6;

int Window::init() {
    glfwSetErrorCallback(GlfwErrorCallback);

    if (!static_cast<bool>(glfwInit())) {
        LOG(FATAL) << "Failed to init glfw!";
        return 1;
    }

    const char* glslVersion = "#version 460";
    LOG(INFO) << "glsl ver: " << glslVersion;
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, kGlfwContextVersionMajor);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, kGlfwContextVersionMinor);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    LOG(INFO) << "Try creating window";
    m_pWindow = glfwCreateWindow(
        m_WindowConfig.width,
        m_WindowConfig.height,
        m_WindowConfig.name.c_str(),
        m_WindowConfig.fullscreen ? glfwGetPrimaryMonitor() : nullptr,
        m_pShareWindow
    );
    if (!static_cast<bool>(m_pWindow)) {
        LOG(FATAL) << "Faield to create window!";
        glfwTerminate();
        return 2;
    }

    glfwMakeContextCurrent(m_pWindow);

    if (!static_cast<bool>(gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))) {
        LOG(FATAL) << "Faield to load gl loader!";
        glfwTerminate();
        return 3;
    }

    IMGUI_CHECKVERSION();

    ImGui::CreateContext();

    auto& io = ImGui::GetIO();
    (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard; // Enable Keyboard Controls
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;  // Enable Gamepad Controls
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;     // Enable Docking
    io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;   // Enable Viewports

    ImGui::StyleColorsDark();

    // setup render backends
    ImGui_ImplGlfw_InitForOpenGL(m_pWindow, true);
#ifdef __EMSCRIPTEN__
    ImGui_ImplGlfw_InstallEmscriptenCanvasResizeCallback("#canvas");
#endif
    ImGui_ImplOpenGL3_Init(glslVersion);

    // setup fonts
    io.Fonts->AddFontDefault();

    auto& fontConfig = m_WindowConfig.fontConfig;
    if (!fontConfig.file.empty()) {
        ImFontConfig imFontConfig{};
        imFontConfig.MergeMode        = true;
        imFontConfig.GlyphMaxAdvanceX = 12.0F;
        constexpr std::array<ImWchar, 16> range{ 0x0020, 0x00FF, 0x2E80, 0x9FFF, 0 };
        imFontConfig.GlyphRanges = range.data();
        LOG(INFO) << fontConfig.file;
        io.Fonts->AddFontFromFileTTF(
            fontConfig.file.c_str(),
            fontConfig.sizePixels,
            &imFontConfig,
            io.Fonts->GetGlyphRangesChineseFull()
        );
        ImGui_ImplOpenGL3_CreateFontsTexture();
    }

    // set callbacks

    glfwSetFramebufferSizeCallback(m_pWindow, GlfwFramebufferCallback);

    glfwSetKeyCallback(m_pWindow, &KeyboardInput::Callback);
    glfwSetCursorPosCallback(m_pWindow, &MouseInput::Callback);

    return (LOG(INFO) << "Successfully initialized the basic things of a window"), onInit();
}

const float kDefaultWindowColor = 0.2F;

void Window::run() {
    LOG(DEBUG) << "Call startup systems";

    startup();

    double lastFrameTime = glfwGetTime();

    LOG(INFO) << "Entering main loop";
    while (!static_cast<bool>(glfwWindowShouldClose(m_pWindow))) {
        // Time calculation
        double currentTime = glfwGetTime();
        float deltaTime    = static_cast<float>(currentTime - lastFrameTime);
        lastFrameTime      = currentTime;

        // Input
        glfwPollEvents();

        glfwSwapInterval(m_WindowConfig.vsync);

        // Prepare for new frame
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        // Update viewport
        glfwGetFramebufferSize(m_pWindow, &m_WindowConfig.width, &m_WindowConfig.height);
        glViewport(0, 0, m_WindowConfig.width, m_WindowConfig.height);

        // Prepare for rendering
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
        glClearColor(kDefaultWindowColor, kDefaultWindowColor, kDefaultWindowColor, 1.0F);

        // Main
        update(deltaTime);

        onConfigPanelsLayout();
        for (auto& [name, panel] : m_Panels) {
            panel->display(deltaTime, m_Worlds);
        }
        afterConfigPanelsLayout();

        // Render
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        // Multi-viewport
        auto& io = ImGui::GetIO();
        if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable) {
            auto* content = glfwGetCurrentContext();
            ImGui::UpdatePlatformWindows();
            ImGui::RenderPlatformWindowsDefault();
            glfwMakeContextCurrent(content);
        }

        // Swap buffers
        glfwSwapBuffers(m_pWindow);

        // Frame control
        if (!m_WindowConfig.vsync) {
            double frameEndTime          = glfwGetTime();
            double frameDuration         = frameEndTime - currentTime;
            const double targetFrameTime = 1.0 / m_WindowConfig.fpsMax;
            if (frameDuration < targetFrameTime) {
                glfwWaitEventsTimeout(targetFrameTime - frameDuration);
            }
        }
    }
}

std::shared_ptr<::atom::ecs::world> Window::createWorld(const std::string& name) {
    if (!m_Worlds.contains(name)) [[likely]] {
        m_Worlds.emplace(name, std::make_shared<ecs::world>());
    }
    else {
        LOG(INFO) << "world [" << name << "] has already exists.";
    }
    return m_Worlds.at(name);
}

atom::map<std::string, std::shared_ptr<::atom::ecs::world>>& Window::worlds() { return m_Worlds; }

void Window::startup() {
    for (auto& [name, world] : m_Worlds) {
        world->startup();
    }
}

void Window::update(float deltaTime) {
    for (auto& [name, world] : m_Worlds) {
        world->update(deltaTime);
    }
}

void Window::shutdown() {
    for (auto& [name, world] : m_Worlds) {
        world->shutdown();
    }
}

void Window::terminate() {
    if (!m_Terminate) {
        m_Terminate = true;

        LOG(INFO) << "Try erminate the window [" << m_WindowConfig.name << "].";

        onTerminate();

        LOG(INFO) << "Shutdown each systems.";
        shutdown();

        LOG(INFO) << "Terminate glfw.";
        glfwTerminate();
    }
}

void SetupWindow(Window& window) {
    if (!window.init()) {
        window.run();
    }
}

} // namespace atom::engine::application
