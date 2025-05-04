#include "KeyboardCallback.hpp"
#include <command.hpp>
#include <queryer.hpp>
#include <world.hpp>
#include "Local.hpp"
#include "components/Camera.hpp"
#include "components/Transform.hpp"

using namespace atom;

void MoveForward() {
    auto command = gWorld->command();
    auto querier = gWorld->query();

    auto& transform = querier.get<components::Transform>(gLocalPlayer);
    auto& camera    = querier.get<components::Camera>(gLocalPlayer);

    // TODO: update postion in both camera and transfrom. forward vector could be get in camera.
}
