#include <easylogging++.h>
#include <imgui.h>
#include <command.hpp>
#include <core/langdef.hpp>
#include <queryer.hpp>
#include "application/KeyboardInput.hpp"
#include "application/Panel.hpp"
#include "application/Window.hpp"
#include "asset.hpp"
#include "io/file.hpp"
#include "platform/path.hpp"
#include "systems/render/BufferObject.hpp"
#include "systems/render/Camera.hpp"
#include "systems/render/Model.hpp"
#include "systems/render/VertexArrayObject.hpp"

using namespace atom::ecs;
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

    static void SwitchConsoleVisibility() {
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

const auto CurrentPath    = atom::engine::io::CurrentPath();
const auto ResourceFolder = CurrentPath / "resources";
const auto ModelsFolder   = ResourceFolder / "models";
const auto LoraPath       = ModelsFolder.string() + sep + "诺菈_by_幻塔" + sep + "诺菈.pmx";
const auto MikuPath       = ModelsFolder.string() + sep + "miku" + sep + "model.pmx";

auto& hub = hub::instance();
auto& lib = ::hub.library<Model>();
auto& tab = ::hub.table<Model>();
Camera gCamera;

static void StartupSys(command& command, queryer& queryer) {
    auto entity = command.spawn<Model>(LoraPath);
    auto& model = queryer.get<Model>(entity);
    auto proxy  = model.load();
    auto handle = lib.install(std::move(proxy));
    model.set_handle(handle);
    tab.emplace(model.path(), handle);
}

static void UpdateSys(command& command, queryer& queryer, float deltaTime) {
    const auto view = gCamera.view();
    const auto proj = gCamera.proj();

    auto entities = queryer.query_any_of<Model>();
    for (const auto entity : entities) {
        auto& model = queryer.get<Model>(entity);
        auto handle = model.get_handle();
        auto proxy  = lib.fetch(handle);
        for (const auto& mesh : proxy->meshes) {
            // mesh.draw();
        }
    }
}

static void ShutdownSys(command& command, queryer& queryer) {
    auto entities = queryer.query_any_of<Model>();
    for (const auto entity : entities) {
        auto& model = queryer.get<Model>(entity);
        auto handle = model.get_handle();
        LOG(INFO) << "unloading handle: [" << handle << "]";
        model.unload();
    }
}

class ApplicationWindow : public Window {
public:
    ApplicationWindow(WindowConfig config) : Window(CheckWindowConfig(std::move(config))) {
        auto& factory = PanelFactory::instance();
        // emplace("Main", factory.make<MainPanel>());
        // pConsole = factory.make<ConsolePanel>();
        // emplace("Console", pConsole);
        auto world = createWorld("main world");
        world->add_startup(StartupSys);
        world->add_update(UpdateSys);
        world->add_shutdown(ShutdownSys);
    }

private:
};

const std::string kConfigPath =
    GetUserHomeDirectroy() + sep + ".atom" + sep + "proj" + sep + "aimer.json";

int main(int argc, char* argv[]) {
    auto config = LoadWindowConfig(kConfigPath);
    CheckWindowConfig(config);
    ApplicationWindow window{ config };
    SetupWindow(window);
    SaveWindowConfig(kConfigPath, config);

    return 0;
}
