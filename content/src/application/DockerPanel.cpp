#include "application/DockerPanel.hpp"
#include <memory>
#include <imgui.h>
#include <world.hpp>

using namespace atom::engine::application;

DockerPanel::DockerPanel(const char* name, const bool open, ImGuiWindowFlags flags)
    : Panel(name, open, flags) {}

DockerPanel::DockerPanel(const std::string& name, const bool open, ImGuiWindowFlags flags)
    : Panel(name, open, flags) {}

DockerPanel::DockerPanel(std::string&& name, const bool open, ImGuiWindowFlags flags)
    : Panel(std::move(name), open, flags) {}

void DockerPanel::onBegin() {
    auto* mainViewport = ImGui::GetWindowViewport();
    ImGui::SetNextWindowPos(mainViewport->Pos);
    ImGui::SetNextWindowSize(mainViewport->Size);
    ImGui::SetNextWindowViewport(mainViewport->ID);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 1.0F);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0F);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0F, 0.0F));
}

void DockerPanel::afterBegin() {
    ImGui::PopStyleVar(3);
    auto dockspaceId = ImGui::GetID("dockspace");
    auto flags       = ImGuiDockNodeFlags_PassthruCentralNode;
    ImGui::DockSpace(dockspaceId, ImVec2(0, 0), flags);
}
