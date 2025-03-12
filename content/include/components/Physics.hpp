#pragma once
#include <BulletCollision/BroadphaseCollision/btDispatcher.h>
#include <btBulletCollisionCommon.h>

namespace components {

struct Physics {
    bool kinematic              = true;
    btRigidBody* body;
};

} // namespace components
