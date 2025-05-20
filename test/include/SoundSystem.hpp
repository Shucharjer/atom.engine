#pragma once

#include <command.hpp>
#include <queryer.hpp>

void StartupTestSoundSystem(atom::ecs::command& command, atom::ecs::queryer& queryer);
void UpdateTestSoundSystem(
    atom::ecs::command& command, atom::ecs::queryer& queryer, float deltaTime
);
void ShutdownTestSoundSystem(atom::ecs::command& command, atom::ecs::queryer& queryer);
