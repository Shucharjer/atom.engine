#pragma once
#include <command.hpp>
#include <ecs.hpp>
#include <queryer.hpp>
#include "components/Camera.hpp"
#include "components/Transform.hpp"

inline atom::ecs::entity::id_t gLocalPlayer;
inline float gLocalSensitivity = 15.f;
inline atom::ecs::world* gWorld;
inline float gDeltaTime;
inline components::Camera* gCamera;
inline bool gEnableMouseInput = true;
inline bool gPlayTheSound     = false;

struct Interactivable {};

inline void CreateLocalPlayer(atom::ecs::command& command, atom::ecs::queryer& queryer) {
    gLocalPlayer = command.spawn<components::Transform>(components::Transform{});
    gWorld       = queryer.current_world();
}

inline void CreateInteractivables(atom::ecs::command& command, atom::ecs::queryer& queryer) {
    command.spawn<Interactivable, components::Transform>(
        Interactivable{
    },
        components::Transform{ .position = { 0.f, 9.f, 12.f } }
    );
}
