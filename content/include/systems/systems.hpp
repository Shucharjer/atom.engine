#include <command.hpp>
#include <queryer.hpp>
#include <world.hpp>

namespace atom::engine::systems {

void StartupRenderSystem(::atom::ecs::command&, ::atom::ecs::queryer&);
void StartupPhysicsSystem(::atom::ecs::command&, ::atom::ecs::queryer&);
void StartupSoundSystem(::atom::ecs::command&, ::atom::ecs::queryer&);
void StartupScriptSystem(::atom::ecs::command&, ::atom::ecs::queryer&);

void UpdateRenderSystem(::atom::ecs::command&, ::atom::ecs::queryer&, float);
void UpdatePhysicsSystem(::atom::ecs::command&, ::atom::ecs::queryer&, float);
void UpdateSoundSystem(::atom::ecs::command&, ::atom::ecs::queryer&, float);
void UpdateScriptSystem(::atom::ecs::command&, ::atom::ecs::queryer&, float);

void ShutdownRenderSystem(::atom::ecs::command&, ::atom::ecs::queryer&);
void ShutdownPhysicsSystem(::atom::ecs::command&, ::atom::ecs::queryer&);
void ShutdownSoundSystem(::atom::ecs::command&, ::atom::ecs::queryer&);
void ShutdownScriptSystem(::atom::ecs::command&, ::atom::ecs::queryer&);

} // namespace atom::engine::systems
