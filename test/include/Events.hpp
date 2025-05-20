#pragma once
#include <LinearMath/btVector3.h>
#include <ecs.hpp>
#include <signal/dispatcher.hpp>
#include <world.hpp>

struct EventCreateBox {
    atom::ecs::entity::id_t entity{};
};

struct PhysicsUserPointer {
    atom::ecs::world* world{};
    atom::ecs::entity::id_t entity{};
};

struct EventCollision {
    atom::ecs::world* world{};
    atom::ecs::entity::id_t entityA{};
    atom::ecs::entity::id_t entityB{};
    btVector3 collisionPoint;
};
