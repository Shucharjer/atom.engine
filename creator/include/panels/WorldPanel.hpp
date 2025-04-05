#pragma once
#include <world.hpp>
#include "application/Panel.hpp"

namespace atom::creator::panels {

class WorldPanel : public engine::application::Panel {
public:
    WorldPanel();

private:
    void layout(
        float deltaTime, atom::map<std::string, std::shared_ptr<::atom::ecs::world>>& worlds
    ) override;

    std::shared_ptr<ecs::world> m_SelectedWorld;
};

} // namespace atom::creator::panels
