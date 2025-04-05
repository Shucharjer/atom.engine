#include <fstream>
#include <memory>
#include <easylogging++.h>
#include <imgui.h>
#include "Paths.hpp"
#include "application/DockerPanel.hpp"
#include "application/Language.hpp"
#include "application/Window.hpp"
#include "panels/ApplicationDockerPanel.hpp"
#include "platform/path.hpp"

using namespace atom::engine::application;
using namespace atom::creator;
using namespace atom::creator::panels;
using namespace atom::engine::platform;

static inline void CheckWindowConfig(WindowConfig& config) noexcept {
    config.name = "Atom Creator";
}

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

std::string GetLanguageFileName() {
    nlohmann::json json;
    std::ifstream stream(kAtomLocalizationFile);
    if (stream.is_open()) [[likely]] {
        try {
            stream >> json;
        }
        catch (...) {
            LOG(ERROR) << "Failed to open localization.json";
        }
    }

    if (auto iter = json.find("filename"); iter != json.end()) {
        return iter.value();
    }
    else {
        LOG(ERROR) << "Couldn't find filename in localization.json";
        return {};
    }
}

int main() {
    LoadLanguageFile(kAtomLocalizationDirectory + sep + GetLanguageFileName());
    auto config = LoadWindowConfig(kAtomDirectory + sep + "window.json");
    CheckWindowConfig(config);
    ApplicationWindow window{ config };
    if (!window.init()) {
        window.run();
    }
    window.terminate();
    SaveWindowConfig(kAtomDirectory + sep + "window.json", config);
    return 0;
}
