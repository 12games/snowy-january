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

//class Trigger;

//class PhysicsObject : public btMotionState
//{
//public:
//    PhysicsObject(class GameObject* obj, float mass);
//    virtual ~PhysicsObject();

//    glm::vec3 position() const;

//	virtual void getWorldTransform(btTransform& worldTrans) const;
//	virtual void setWorldTransform(const btTransform& worldTrans);

//    class GameObject* _obj;
//    btCollisionShape* _shape;
//    btTransform _transform;
//    btRigidBody* _rigidBody;
//};

class PhysicsManager
{
private:
    btBroadphaseInterface *_broadphase;
    btDefaultCollisionConfiguration *_collisionConfiguration;
    btCollisionDispatcher *_dispatcher;
    btSequentialImpulseConstraintSolver *_solver;
    btDiscreteDynamicsWorld *_dynamicsWorld;

    static struct Config
    {
        float _gravity;

    } _config;

public:
    PhysicsManager();
    virtual ~PhysicsManager();

    void Step(float gameTime);

    void AddObject(PhysicsObject *obj, short group = btBroadphaseProxy::DefaultFilter, short mask = btBroadphaseProxy::DefaultFilter | btBroadphaseProxy::StaticFilter | btBroadphaseProxy::CharacterFilter);
    void RemoveObject(PhysicsObject *obj);
};

#endif /* PHYSICS_H */
