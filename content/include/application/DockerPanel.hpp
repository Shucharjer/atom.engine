#pragma once
#include "application/Panel.hpp"

namespace atom::engine::application {

class DockerPanel : public Panel {
public:
    DockerPanel(const char* name, const bool open = true, ImGuiWindowFlags flags = 0);
    DockerPanel(const std::string& name, const bool open = true, ImGuiWindowFlags flags = 0);
    DockerPanel(std::string&& name, const bool open = true, ImGuiWindowFlags flags = 0);

protected:
    void onBegin() override;
    void afterBegin() override;
};

} // namespace atom::engine::application
