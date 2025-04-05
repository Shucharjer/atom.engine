#include "panels/WorldPanel.hpp"
#include <world.hpp>

using namespace atom::creator;
using namespace atom::creator::panels;
using namespace atom::engine;
using namespace atom::engine::application;

WorldPanel::WorldPanel() : Panel("world") {};

void WorldPanel::layout(
    float deltaTime, atom::map<std::string, std::shared_ptr<atom::ecs::world>>& worlds
) {}
