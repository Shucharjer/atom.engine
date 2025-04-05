#include "panels/ApplicationDockerPanel.hpp"
#include <core/langdef.hpp>
#include <pchs/graphics.hpp>

#include <memory>
#include <imgui.h>
#include <tinyfiledialogs.h>
#include <application/DockerPanel.hpp>
#include <application/Language.hpp>
#include <world.hpp>
#include "Paths.hpp"
#include "platform/path.hpp"

using namespace atom::creator::panels;
using namespace atom::ecs;
using namespace atom::engine::application;

constexpr auto kApplicationDockerWindowFlags =
    ImGuiWindowFlags_NoDocking | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse |
    ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoBackground |
    ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus | ImGuiWindowFlags_MenuBar;

constexpr auto kDockerString = R"(
{
    "self": "docker",
    "project": {
        "self": "Project",
        "openProject": "Open project",
        "createNewProject": "Create new project",
        "saveProject": "Save project",
        "saveAs": "Save as"
    },
    "about": {
        "self": "About"
    }"
}
)";

static inline void SetDockerText() noexcept {
    auto& lang = GetLanguage();
    if (auto iter = lang.find("docker"); iter == lang.end()) {
        iter.value() = kDockerString;
    }
}

ApplicationDockerPanel::ApplicationDockerPanel()
    : DockerPanel("docker", true, kApplicationDockerWindowFlags) {
    SetDockerText();
}

const std::array<const char*, 1> filters_ = { "*.atomproj" };

void ApplicationDockerPanel::layout(
    float deltaTime, atom::map<std::string, std::shared_ptr<world>>& worlds
) {
    auto& lang              = GetLanguage();
    nlohmann::json& docker  = lang.at("docker");
    nlohmann::json* content = nullptr;
    std::string temp;

    ImGui::BeginMainMenuBar();

    content = &docker.at("project");
    temp    = (*content)["self"];
    if (ImGui::BeginMenu(temp.c_str())) {
        temp = (*content)["openProject"];
        if (ImGui::MenuItem(temp.c_str())) {
            const char* selected_file = tinyfd_openFileDialog(
                temp.c_str(),
                (kAtomProjectsDirectory + engine::platform::sep).data(),
                1,
                static_cast<const char* const*>(filters_.data()),
                nullptr,
                0
            );
        }

        temp = (*content)["saveProject"];
        if (ImGui::MenuItem(temp.c_str())) {}

        temp = (*content)["saveAs"];
        if (ImGui::MenuItem(temp.c_str())) {}

        ImGui::EndMenu();
    }

    content = &docker.at("about");
    temp    = (*content)["self"];
    if (ImGui::BeginMenu(temp.c_str())) {

        ImGui::EndMenu();
    }

    ImGui::EndMainMenuBar();
}
