#include "PhysicsSystem.hpp"
#include <BulletCollision/BroadphaseCollision/btBroadphaseInterface.h>
#include <BulletCollision/BroadphaseCollision/btDbvtBroadphase.h>
#include <BulletCollision/BroadphaseCollision/btDispatcher.h>
#include <BulletCollision/CollisionDispatch/btCollisionDispatcher.h>
#include <BulletCollision/CollisionDispatch/btCollisionObject.h>
#include <BulletCollision/CollisionDispatch/btDefaultCollisionConfiguration.h>
#include <BulletCollision/CollisionShapes/btCapsuleShape.h>
#include <BulletCollision/CollisionShapes/btCollisionShape.h>
#include <BulletCollision/CollisionShapes/btSphereShape.h>
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

const auto kDefaultGravity = btVector3(0.f, -10.f, 0.f);

static btAlignedObjectArray<btCollisionShape*> collisionShapes;

static btRigidBody* sRigidBody;

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

    using RConstructionInfo = btRigidBody::btRigidBodyConstructionInfo;

    auto interactives = queryer.query_all_of<Interactivable, components::Transform>();

    for (auto interactive : interactives) {
        auto& transform               = queryer.get<components::Transform>(interactive);
        btCollisionShape* sphereShape = new btSphereShape(1.0f);
        const float sphereMass        = 1.f;
        btVector3 sphereInertia;
        sphereShape->calculateLocalInertia(sphereMass, sphereInertia);
        auto* sphereMotionState = new btDefaultMotionState(btTransform(
            btQuaternion(0, 0, 0, 1),
            btVector3(transform.position.x, transform.position.y, transform.position.z)
        ));
        RConstructionInfo sphereConstructionInfo{
            sphereMass, sphereMotionState, sphereShape, sphereInertia
        };
        auto* sphereRigidBody = new btRigidBody{ sphereConstructionInfo };
        sRigidBody            = sphereRigidBody;
        collisionShapes.push_back(sphereShape);
        dynamicsWorld->addRigidBody(sphereRigidBody);
    }
}

static void UpdateTransform(btRigidBody& rigidBody, components::Transform& transform) {
    auto& btTransform    = rigidBody.getWorldTransform();
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

void UpdateTestPhysicsSystem(
    atom::ecs::command& command, atom::ecs::queryer& queryer, float deltaTime
) {
    using namespace components;
    dynamicsWorld->stepSimulation(deltaTime, 10);

    // auto entities = queryer.query_all_of<Transform, btRigidBody>();

    // for (auto entity : entities) {
    //     auto& body      = queryer.get<btRigidBody>(entity);
    //     auto& transform = queryer.get<Transform>(entity);
    //     UpdateTransform(body, transform);
    // }

    auto interactives = queryer.query_all_of<Interactivable, Transform>();
    for (auto interactive : interactives) {
        auto& transform = queryer.get<Transform>(interactive);
        UpdateTransform(*sRigidBody, transform);
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
