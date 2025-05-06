#include "PhysicsSystem.hpp"
#include <BulletCollision/BroadphaseCollision/btBroadphaseInterface.h>
#include <BulletCollision/BroadphaseCollision/btDbvtBroadphase.h>
#include <BulletCollision/CollisionDispatch/btCollisionDispatcher.h>
#include <BulletCollision/CollisionDispatch/btDefaultCollisionConfiguration.h>
#include <BulletDynamics/ConstraintSolver/btSequentialImpulseConstraintSolver.h>
#include <BulletDynamics/Dynamics/btDiscreteDynamicsWorld.h>
#include <LinearMath/btVector3.h>
#include <btBulletDynamicsCommon.h>
#include "Local.hpp"
#include "components/Transform.hpp"

static btDefaultCollisionConfiguration* collisionConfiguration = nullptr;
static btCollisionDispatcher* dispatcher                       = nullptr;
static btBroadphaseInterface* broadphase                       = nullptr;
static btSequentialImpulseConstraintSolver* solver             = nullptr;
static btDiscreteDynamicsWorld* dynamicsWorld                  = nullptr;

const auto kDefaultGravity = btVector3(0.f, -9.8f, 0.f);

void StartupTestPhysicsSystem(atom::ecs::command&, atom::ecs::queryer&) {
    collisionConfiguration = new btDefaultCollisionConfiguration;
    dispatcher             = new btCollisionDispatcher(collisionConfiguration);
    broadphase             = new btDbvtBroadphase;
    solver                 = new btSequentialImpulseConstraintSolver;
    dynamicsWorld =
        new btDiscreteDynamicsWorld(dispatcher, broadphase, solver, collisionConfiguration);
    dynamicsWorld->setGravity(kDefaultGravity);
}

void UpdateTestPhysicsSystem(
    atom::ecs::command& command, atom::ecs::queryer& queryer, float deltaTime
) {
    dynamicsWorld->stepSimulation(deltaTime, 10);

    auto entities = queryer.query_all_of<components::Transform>();
}

void ShutdownTestPhysicsSystem(atom::ecs::command&, atom::ecs::queryer&) {

    delete collisionConfiguration;
    delete dispatcher;
    delete broadphase;
    delete solver;
    delete dynamicsWorld;
}
