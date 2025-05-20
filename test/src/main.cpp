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
#include "Local.hpp"
#include "PhysicsSystem.hpp"
#include "RenderSystem.hpp"
#include "SoundSystem.hpp"
#include "application/KeyboardInput.hpp"
#include "application/Panel.hpp"
#include "application/Window.hpp"
#include "asset.hpp"
#include "platform/path.hpp"
#include "systems/render/Model.hpp"

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

auto& hub = hub::instance();
auto& lib = ::hub.library<Model>();
auto& tab = ::hub.table<Model>();

static uint32_t highest = 0;
static uint32_t lowest  = 1 << 31;
static uint64_t total;
static uint32_t seconds = 0;
static uint32_t frame;
static float accumulateTime = 0;
static void updateFrame(command&, queryer&, float deltaTime) {
    ++frame;
    accumulateTime += deltaTime;
    if (accumulateTime >= 1.0) {
        ++seconds;
        highest = std::max(highest, frame);
        lowest  = std::min(lowest, frame);
        total += frame;
        LOG(INFO) << "current fps: " << frame << ", avg: " << (float)total / (float)seconds
                  << ", h: " << highest << ", l: " << lowest;
        accumulateTime = 0.0;
        frame          = 0;
    }
}

class ApplicationWindow : public Window {
public:
    ApplicationWindow(WindowConfig config) : Window(CheckWindowConfig(std::move(config))) {
        auto& factory = PanelFactory::instance();
        auto world    = createWorld("main world");
        world->add_startup(&CreateLocalPlayer, atom::ecs::early_main_thread);

        world->add_startup(StartupTestRenderSystem, atom::ecs::late_main_thread);
        world->add_update(UpdateTestRenderSystem, atom::ecs::late_main_thread);
        world->add_shutdown(ShutdownTestRenderSystem, atom::ecs::late_main_thread);

        world->add_startup(CreateInteractivables, atom::ecs::early_main_thread);

        world->add_update(updateFrame);

        world->add_startup(StartupTestPhysicsSystem, atom::ecs::late_main_thread);
        world->add_update(UpdateTestPhysicsSystem);
        world->add_shutdown(ShutdownTestPhysicsSystem);

        world->add_startup(StartupTestSoundSystem, atom::ecs::late_main_thread);
        world->add_update(UpdateTestSoundSystem, atom::ecs::late_main_thread);
        world->add_shutdown(ShutdownTestSoundSystem, atom::ecs::late_main_thread);
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
