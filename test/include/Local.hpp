#pragma once
#include <command.hpp>
#include <ecs.hpp>
#include <queryer.hpp>
#include "components/Camera.hpp"
#include "components/Transform.hpp"

inline atom::ecs::entity::id_t gLocalPlayer;
inline float gLocalSensitivity = 1.f;
inline atom::ecs::world* gWorld;
inline float gDeltaTime;
inline components::Camera* gCamera;
inline bool gEnableMouseInput = true;
inline bool gPlayTheSound     = false;

inline void CreateLocalPlayer(atom::ecs::command& command, atom::ecs::queryer& queryer) {
    gLocalPlayer = command.spawn<components::Transform>(components::Transform{});
    gWorld       = queryer.current_world();
}
