#include "KeyboardCallback.hpp"
#include <command.hpp>
#include <queryer.hpp>
#include <world.hpp>
#include "Local.hpp"
#include "components/Camera.hpp"
#include "components/Transform.hpp"

using namespace atom;

const float move_speed = 5.0F;

void MoveForward() {
    auto command    = gWorld->command();
    auto querier    = gWorld->query();
    auto& transform = querier.get<components::Transform>(gLocalPlayer);
    auto& camera    = querier.get<components::Camera>(gLocalPlayer);

    // TODO: update postion in both camera and transfrom. forward vector could be get in camera.
    transform.position += camera.forward * gDeltaTime * move_speed;
}

void MoveBackward() {
    auto command = gWorld->command();
    auto querier = gWorld->query();

    auto& transform = querier.get<components::Transform>(gLocalPlayer);
    auto& camera    = querier.get<components::Camera>(gLocalPlayer);

    transform.position -= camera.forward * gDeltaTime * move_speed;
}
