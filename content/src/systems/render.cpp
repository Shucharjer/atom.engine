#include "systems/systems.hpp"

using namespace atom::ecs;

namespace atom::engine::systems {

void StartupRenderSystem(command& command, queryer& queryer) {}

void UpdateRenderSystem(command& command, queryer& queryer, float deltaTime) {}

void ShutdownRenderSystem(command& command, queryer& queryer) {}

} // namespace atom::engine::systems
