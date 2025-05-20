#include "PhysicsSystem.hpp"
#include <BulletCollision/BroadphaseCollision/btBroadphaseInterface.h>
#include <BulletCollision/BroadphaseCollision/btDbvtBroadphase.h>
#include <BulletCollision/BroadphaseCollision/btDispatcher.h>
#include <BulletCollision/CollisionDispatch/btCollisionDispatcher.h>
#include <BulletCollision/CollisionDispatch/btCollisionObject.h>
#include <BulletCollision/CollisionDispatch/btDefaultCollisionConfiguration.h>
#include <BulletCollision/CollisionShapes/btCapsuleShape.h>
#include <BulletCollision/CollisionShapes/btCollisionShape.h>
#include <BulletCollision/CollisionShapes/btStaticPlaneShape.h>
#include <BulletCollision/NarrowPhaseCollision/btPersistentManifold.h>
#include <BulletDynamics/ConstraintSolver/btSequentialImpulseConstraintSolver.h>
#include <BulletDynamics/Dynamics/btDiscreteDynamicsWorld.h>
#include <BulletDynamics/Dynamics/btDynamicsWorld.h>
#include <BulletDynamics/Dynamics/btRigidBody.h>
#include <LinearMath/btAlignedObjectArray.h>
#include <LinearMath/btDefaultMotionState.h>
#include <LinearMath/btQuaternion.h>
#include <LinearMath/btScalar.h>
#include <LinearMath/btTransform.h>
#include <LinearMath/btVector3.h>
#include <btBulletDynamicsCommon.h>
#include <command.hpp>
#include <queryer.hpp>
#include "Events.hpp"
#include "Local.hpp"
#include "components/Transform.hpp"
#include "event.hpp"

static btDefaultCollisionConfiguration* collisionConfiguration = nullptr;
static btCollisionDispatcher* dispatcher                       = nullptr;
static btBroadphaseInterface* broadphase                       = nullptr;
static btSequentialImpulseConstraintSolver* solver             = nullptr;
static btDiscreteDynamicsWorld* dynamicsWorld                  = nullptr;

static btCollisionShape* playerShape;

const auto kDefaultGravity = btVector3(0.f, -9.8f, 0.f);

static btAlignedObjectArray<btCollisionShape*> collisionShapes;

void StartupTestPhysicsSystem(atom::ecs::command& command, atom::ecs::queryer& queryer) {
    collisionConfiguration = new btDefaultCollisionConfiguration;
    dispatcher             = new btCollisionDispatcher(collisionConfiguration);
    broadphase             = new btDbvtBroadphase;
    solver                 = new btSequentialImpulseConstraintSolver;
    dynamicsWorld =
        new btDiscreteDynamicsWorld(dispatcher, broadphase, solver, collisionConfiguration);
    dynamicsWorld->setGravity(kDefaultGravity);

    btCollisionShape* groundShape = new btStaticPlaneShape(btVector3(0, 1, 0), 0);
    btDefaultMotionState* groundMotionState =
        new btDefaultMotionState(btTransform(btQuaternion(0, 0, 0, 1), btVector3(0, 0, 0)));
    btRigidBody::btRigidBodyConstructionInfo groundInfo(
        0, groundMotionState, groundShape, btVector3(0, 0, 0)
    );

    btRigidBody* groundBody = new btRigidBody(groundInfo);
    collisionShapes.push_back(groundShape);
    dynamicsWorld->addRigidBody(groundBody);
}

void callback(btDynamicsWorld* world, btScalar timeStep) {
    btDispatcher* dispatcher = world->getDispatcher();
    auto numManifolds        = dispatcher->getNumManifolds();

    for (auto i = 0; i < numManifolds; ++i) {
        btPersistentManifold* manifold = dispatcher->getManifoldByIndexInternal(i);

        if (manifold->getNumContacts() > 0) {
            const btCollisionObject* objA = manifold->getBody0();
            const btCollisionObject* objB = manifold->getBody1();

            auto* userPointer = objA->getUserPointer();
        }
    }
}

void UpdateTestPhysicsSystem(
    atom::ecs::command& command, atom::ecs::queryer& queryer, float deltaTime
) {
    using namespace components;
    dynamicsWorld->stepSimulation(deltaTime, 10);

    auto entities = queryer.query_all_of<Transform, btRigidBody>();

    for (auto entity : entities) {
        auto& body           = queryer.get<btRigidBody>(entity);
        auto& transform      = queryer.get<Transform>(entity);
        auto& btTransform    = body.getWorldTransform();
        auto& origin         = btTransform.getOrigin();
        transform.position.x = origin.x();
        transform.position.y = origin.y();
        transform.position.z = origin.z();
        auto btRot           = btTransform.getRotation();
        transform.rotation.w = btRot.w();
        transform.rotation.x = btRot.x();
        transform.rotation.y = btRot.y();
        transform.rotation.z = btRot.z();
    }
}

void ShutdownTestPhysicsSystem(atom::ecs::command& command, atom::ecs::queryer& queryer) {

    for (auto i = dynamicsWorld->getNumCollisionObjects() - 1; i >= 0; --i) {
        btCollisionObject* obj = dynamicsWorld->getCollisionObjectArray()[i];
        auto* body             = btRigidBody::upcast(obj);
        if (body && body->getMotionState()) {
            delete body->getMotionState();
        }
        dynamicsWorld->removeCollisionObject(obj);
    }

    for (auto i = 0; i < collisionShapes.size(); ++i) {
        btCollisionShape* shape = collisionShapes[i];
        collisionShapes[i]      = 0;
        delete shape;
    }

    delete collisionConfiguration;
    delete dispatcher;
    delete broadphase;
    delete solver;
    delete dynamicsWorld;

    collisionShapes.clear();
}
