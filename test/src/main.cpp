#include <easylogging++.h>
#include <imgui.h>
#include <stb_image.h>
#include <stb_image_write.h>
#include <winscard.h>
#include <command.hpp>
#include <core/langdef.hpp>
#include <glm/ext/matrix_transform.hpp>
#include <queryer.hpp>
#include <world.hpp>
#include "KeyboardCallback.hpp"
#include "Local.hpp"
#include "MouseCallback.hpp"
#include "PhysicsSystem.hpp"
#include "RenderSystem.hpp"
#include "application/KeyboardInput.hpp"
#include "application/MouseInput.hpp"
#include "application/Panel.hpp"
#include "application/Window.hpp"
#include "asset.hpp"
#include "io/file.hpp"
#include "pchs/graphics.hpp"
#include "pchs/math.hpp"
#include "platform/path.hpp"
#include "systems/render/BufferObject.hpp"
#include "systems/render/Camera.hpp"
#include "systems/render/Framebuffer.hpp"
#include "systems/render/Model.hpp"
#include "systems/render/ShaderProgram.hpp"
#include "systems/render/Transform.hpp"
#include "systems/render/VertexArrayObject.hpp"

using namespace atom::ecs;
using namespace atom::engine;
using namespace atom::engine::application;
using namespace atom::engine::platform;
using namespace atom::engine::systems::render;

ATOM_FORCE_INLINE static void CheckWindowConfig(WindowConfig& config) { config.name = "Aimer"; }

ATOM_FORCE_INLINE static WindowConfig CheckWindowConfig(WindowConfig&& config) {
    WindowConfig cfg{ std::move(config) };
    cfg.name = "Aimer";
    return config;
}

class ConsolePanel;

ConsolePanel* pConsole;

class ConsolePanel final : public Panel {
public:
    ConsolePanel() : Panel("Console", false, ImGuiWindowFlags_NoSavedSettings) {
        auto& inst = KeyboardInput::instance();
        inst.setPressCallback(GLFW_KEY_W, SwitchConsoleVisibility);
    }

    static void SwitchConsoleVisibility(GLFWwindow*) {
        bool display = pConsole->getDisplay();
        pConsole->setDisplay(!display);
    }

private:
    void onBegin() override {}
    void layout(
        float deltaTime, atom::map<std::string, std::shared_ptr<::atom::ecs::world>>& worlds
    ) override {}
};

class MainPanel final : public Panel {
public:
    MainPanel()
        : Panel(
              "Main",
              true,
              ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoTitleBar |
                  ImGuiWindowFlags_NoResize
          ) {}

private:
    void onBegin() override {
        auto* mainViewport = ImGui::GetWindowViewport();
        ImGui::SetNextWindowViewport(mainViewport->ID);
        ImGui::SetNextWindowPos(mainViewport->Pos);
        ImGui::SetNextWindowSize(mainViewport->Size);
    }

    void layout(
        float deltaTime, atom::map<std::string, std::shared_ptr<::atom::ecs::world>>& worlds
    ) override {
        constexpr auto kStart = "Start";
        auto buttonSize       = ImGui::CalcTextSize(kStart);
        auto& style           = ImGui::GetStyle();
        buttonSize.x += style.FramePadding.x * 2;
        buttonSize.y += style.FramePadding.y * 2;

        auto context = ImGui::GetCurrentContext();

        auto windowPos = ImGui::GetWindowPos();
    }
};

auto& hub = hub::instance();
auto& lib = ::hub.library<Model>();
auto& tab = ::hub.table<Model>();

static void StartupPhysicsSystem(command& command, queryer& queryer) {}

static void UpdatePhysicsSystem(command& command, queryer& queryer, float deltaTime) {}

static void ShutdownPhysicsSystem(command& command, queryer& queryer) {}

class ApplicationWindow : public Window {
public:
    ApplicationWindow(WindowConfig config) : Window(CheckWindowConfig(std::move(config))) {
        auto& factory = PanelFactory::instance();
        // emplace("Main", factory.make<MainPanel>());
        // pConsole = factory.make<ConsolePanel>();
        // emplace("Console", pConsole);
        auto world = createWorld("main world");
        world->add_startup(StartupTestRenderSystem, atom::ecs::late_main_thread);
        world->add_update(UpdateTestRenderSystem, atom::ecs::late_main_thread);
        world->add_shutdown(ShutdownTestRenderSystem, atom::ecs::late_main_thread);

        world->add_startup(StartupTestPhysicsSystem, atom::ecs::late_main_thread);
        world->add_update(UpdateTestPhysicsSystem, atom::ecs::late_main_thread);
        world->add_shutdown(ShutdownTestPhysicsSystem, atom::ecs::late_main_thread);
    }

private:
};

const std::string kConfigPath =
    GetUserHomeDirectroy() + sep + ".atom" + sep + "proj" + sep + "aimer.json";

int main(int argc, char* argv[]) {
    auto config = LoadWindowConfig(kConfigPath);
    ApplicationWindow window{ std::move(config) };
    SetupWindow(window);
    SaveWindowConfig(kConfigPath, window.config());

    return 0;
}
