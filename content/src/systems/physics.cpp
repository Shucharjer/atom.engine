#include <algorithm>
#include <thread>
#include <command.hpp>
#include <queryer.hpp>
#include "systems/systems.hpp"

using namespace atom::ecs;

namespace atom::engine::systems {

void StartupPhysicsSystem(command& command, queryer& queryer) {}

ATOM_FORCE_INLINE static void SyncWorldToPhysics(command& command, queryer& queryer) {}

void UpdatePhysicsSystem(command& command, queryer& queryer, float deltaTime) {
    // four things need to do:
    // 1. sync world to physics
    // 2. simulate
    // 3. sync physics to world
    // 4. process result
    SyncWorldToPhysics(command, queryer);
}

void ShutdownPhysicsSystem(command& command, queryer& queryer) {}

} // namespace systems
