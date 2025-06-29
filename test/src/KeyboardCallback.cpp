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

void MoveForward(GLFWwindow*) { gCamera->position += gCamera->forward * 1 * move_speed; }

void MoveBackward(GLFWwindow*) { gCamera->position -= gCamera->forward * 1 * move_speed; }

void MoveLeft(GLFWwindow*) { gCamera->position += gCamera->left * 1 * move_speed; }

void MoveRight(GLFWwindow*) { gCamera->position -= gCamera->left * 1 * move_speed; }

void MoveJump(GLFWwindow*) { gCamera->position += gCamera->up * 1 * move_speed; }

void MoveDown(GLFWwindow*) { gCamera->position -= gCamera->up * 1 * move_speed; }

void RotationUp(GLFWwindow*) { gCamera->rotate(rotation_speed, engine::math::Vector3(1, 0, 0)); }
void RotationDown(GLFWwindow*) { gCamera->rotate(-rotation_speed, engine::math::Vector3(1, 0, 0)); }
void RotationLeft(GLFWwindow*) { gCamera->rotate(rotation_speed, engine::math::Vector3(0, 1, 0)); }
void RotationRight(GLFWwindow*) {
    gCamera->rotate(-rotation_speed, engine::math::Vector3(0, 1, 0));
}
void RotationX(GLFWwindow*) { gCamera->rotate(rotation_speed, engine::math::Vector3(1, 0, 0)); }
