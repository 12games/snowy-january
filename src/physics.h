/* 
 * File:   Physics.h
 * Author: Administrator
 *
 * Created on 28 oktober 2010, 2:08
 */

#ifndef PHYSICS_H
#define PHYSICS_H

#include <BulletDynamics/Dynamics/btRigidBody.h>
#include <LinearMath/btIDebugDraw.h>
#include <LinearMath/btMotionState.h>
#include <btBulletDynamicsCommon.h>

#include "physicsobject.h"

class PhysicsManager
{
public:
    PhysicsManager();

    virtual ~PhysicsManager();

    void InitDebugDraw();

    void DebugDraw(
        glm::mat4 const &proj,
        glm::mat4 const &view);

    void Step(
        float gameTime);

    void AddObject(
        PhysicsObject *obj,
        short group = btBroadphaseProxy::DefaultFilter,
        short mask = btBroadphaseProxy::DefaultFilter | btBroadphaseProxy::StaticFilter | btBroadphaseProxy::CharacterFilter);

    void RemoveObject(
        PhysicsObject *obj);

private:
    friend class PhysicsObjectBuilder;
    btBroadphaseInterface *_broadphase = nullptr;
    btDefaultCollisionConfiguration *_collisionConfiguration = nullptr;
    btCollisionDispatcher *_dispatcher = nullptr;
    btSequentialImpulseConstraintSolver *_solver = nullptr;
    btDiscreteDynamicsWorld *_dynamicsWorld = nullptr;

    static struct Config
    {
        float _gravity;

    } _config;

    class DebugDrawer *_drawer = nullptr;
};

#endif /* PHYSICS_H */
