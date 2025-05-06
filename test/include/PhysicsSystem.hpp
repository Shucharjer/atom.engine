#pragma once
#include <command.hpp>
#include <queryer.hpp>
extern void StartupTestPhysicsSystem(atom::ecs::command&, atom::ecs::queryer&);
extern void UpdateTestPhysicsSystem(atom::ecs::command&, atom::ecs::queryer&, float);
extern void ShutdownTestPhysicsSystem(atom::ecs::command&, atom::ecs::queryer&);
