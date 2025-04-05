#pragma once
#include <application/DockerPanel.hpp>

namespace atom::creator::panels {

class ApplicationDockerPanel : public engine::application::DockerPanel {
public:
    explicit ApplicationDockerPanel();

    void layout(float deltaTime, atom::map<std::string, std::shared_ptr<atom::ecs::world>>& worlds)
        override;
};

} // namespace atom::creator::panels
