#include <memory>
#include <imgui.h>
#include "application/DockerPanel.hpp"
#include "application/Language.hpp"
#include "application/Window.hpp"


using namespace atom::engine::application;

const auto kApplicationDockerWindowFlags =
    ImGuiWindowFlags_NoDocking | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse |
    ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoBackground |
    ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus | ImGuiWindowFlags_MenuBar;

class ApplicationDockerPanel : public DockerPanel {
public:
    explicit ApplicationDockerPanel()
        : DockerPanel("docker", true, kApplicationDockerWindowFlags) {}

    void layout(float deltaTime, std::map<std::string, std::shared_ptr<atom::ecs::world>>& worlds)
        override {
        auto& lang = GetLanguage();

        ImGui::BeginMainMenuBar();

        // ImGui::BeginMenu();
        // ImGui::EndMenu();

        ImGui::EndMainMenuBar();
    }
};

class ApplicationWindow : public Window {
public:
    explicit ApplicationWindow(
        WindowConfig config = WindowConfig{}, GLFWwindow* shareWindow = nullptr
    )
        : Window(std::move(config), shareWindow) {
        auto& factory     = PanelFactory::instance();
        auto* dockerPanel = factory.make<ApplicationDockerPanel>();
        emplace("docker", dockerPanel);
    }
};

int main() {
    auto config = LoadWindowConfig("");
    ApplicationWindow window{ config };
    if (!window.init()) {
        window.run();
    }
    window.terminate();
    return 0;
}
