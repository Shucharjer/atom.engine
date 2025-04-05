#include "application/PerformancePanel.hpp"
#include <memory>
#include <imgui.h>

using namespace atom::engine::application;

const float kThousand = 1000.F;

void PerformancePanel::layout(
    float deltaTime, atom::map<std::string, std::shared_ptr<::atom::ecs::world>>& worlds
) {
    ImGui::Text("FPS: %.0f", ImGui::GetIO().Framerate);
    ImGui::Text("Frame Time: %.3f ms", deltaTime * kThousand);
}
