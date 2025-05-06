#pragma once
#include <command.hpp>
#include <queryer.hpp>

extern void StartupTestRenderSystem(atom::ecs::command&, atom::ecs::queryer&);
extern void UpdateTestRenderSystem(atom::ecs::command&, atom::ecs::queryer&, float deltaTime);
extern void ShutdownTestRenderSystem(atom::ecs::command&, atom::ecs::queryer&);
