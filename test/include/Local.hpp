#pragma once
#include <ecs.hpp>
#include "components/Camera.hpp"

inline atom::ecs::entity::id_t gLocalPlayer;
inline float gLocalSensitivity = 1.f;
inline atom::ecs::world* gWorld;
inline float gDeltaTime;
inline components::Camera* gCamera;
