#pragma once
#include <memory>
#include <world.hpp>
#include "application/Panel.hpp"

namespace atom::engine::application {

class PerformancePanel : public Panel {
public:
    PerformancePanel() : Panel("Performance") {}

private:
    void layout(float deltaTime, std::map<std::string, std::shared_ptr<::atom::ecs::world>>& worlds)
        override;
};

} // namespace atom::engine::application
