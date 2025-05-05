#include "KeyboardCallback.hpp"
#include <easylogging++.h>
#include <command.hpp>
#include <queryer.hpp>
#include <world.hpp>
#include "Local.hpp"
#include "pchs/math.hpp"

using namespace atom;

const float move_speed     = 5.0F;
const float rotation_speed = 30.0F;

void printTransform() {
    auto eulerAngles = gCamera->eulerAngles();
    const auto view  = gCamera->view();
    LOG(INFO) << "position = " << gCamera->position.x << "," << gCamera->position.y << ","
              << gCamera->position.z << "  eulerAngles = " << eulerAngles.x << "," << eulerAngles.y
              << "," << eulerAngles.z << "\n"
              << "  forward = " << gCamera->forward.x << "," << gCamera->forward.y << ","
              << gCamera->forward.z << "  orientation = " << gCamera->orientation.w << ","
              << gCamera->orientation.x << "," << gCamera->orientation.y << ","
              << gCamera->orientation.z << "\n";
}

void MoveForward() {
    gCamera->position += gCamera->forward * 1 * move_speed;
    printTransform();
}

void MoveBackward() {
    gCamera->position -= gCamera->forward * 1 * move_speed;
    printTransform();
}

void MoveLeft() {
    gCamera->position += gCamera->left * 1 * move_speed;
    printTransform();
}

void MoveRight() {
    gCamera->position -= gCamera->left * 1 * move_speed;
    printTransform();
}

void MoveJump() {
    gCamera->position += gCamera->up * 1 * move_speed;
    printTransform();
}

void MoveDown() {
    gCamera->position -= gCamera->up * 1 * move_speed;
    printTransform();
}

void RotationUp() {
    gCamera->rotate(rotation_speed, engine::math::Vector3(1, 0, 0));
    printTransform();
}
void RotationDown() {
    gCamera->rotate(-rotation_speed, engine::math::Vector3(1, 0, 0));
    printTransform();
}
void RotationLeft() {
    gCamera->rotate(rotation_speed, engine::math::Vector3(0, 1, 0));
    printTransform();
}
void RotationRight() {
    gCamera->rotate(-rotation_speed, engine::math::Vector3(0, 1, 0));
    printTransform();
}
void RotationX() {
    gCamera->rotate(rotation_speed, engine::math::Vector3(1, 0, 0));
    printTransform();
}
