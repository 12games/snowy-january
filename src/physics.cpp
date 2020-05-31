#include "physics.h"
#include <glm/gtc/type_ptr.hpp>
#include <iostream>
#include <vector>

using namespace std;

PhysicsManager::Config PhysicsManager::_config = {9.81f};

PhysicsManager::PhysicsManager()
    : _drawer(nullptr)
{
    _broadphase = new btDbvtBroadphase();

    _collisionConfiguration = new btDefaultCollisionConfiguration();
    _dispatcher = new btCollisionDispatcher(_collisionConfiguration);

    _solver = new btSequentialImpulseConstraintSolver();

    _dynamicsWorld = new btDiscreteDynamicsWorld(_dispatcher, _broadphase, _solver, _collisionConfiguration);
    _dynamicsWorld->setGravity(btVector3(0, 0, -PhysicsManager::_config._gravity));
}

PhysicsManager::~PhysicsManager()
{
    if (_dynamicsWorld != nullptr)
    {
        delete _dynamicsWorld;
    }
    _dynamicsWorld = nullptr;

    if (_solver != nullptr)
    {
        delete _solver;
    }
    _solver = nullptr;

    if (_dispatcher != nullptr)
    {
        delete _dispatcher;
    }
    _dispatcher = nullptr;

    if (_collisionConfiguration != nullptr)
    {
        delete _collisionConfiguration;
    }
    _collisionConfiguration = nullptr;

    if (_broadphase != nullptr)
    {
        delete _broadphase;
    }
    _broadphase = nullptr;
}

void PhysicsManager::Step(float gameTime)
{
    _dynamicsWorld->stepSimulation(gameTime, 1);
    int numManifolds = _dynamicsWorld->getDispatcher()->getNumManifolds();

    for (int i = 0; i < numManifolds; i++)
    {
        btPersistentManifold *contactManifold = _dynamicsWorld->getDispatcher()->getManifoldByIndexInternal(i);

        if (contactManifold->getNumContacts() <= 0) continue;

        if (contactManifold->getBody0()->getUserPointer() == nullptr) continue;
        if (contactManifold->getBody1()->getUserPointer() == nullptr) continue;

        //        auto entityA = static_cast<GameObject *>(contactManifold->getBody0()->getUserPointer());
        //        if (entityA == nullptr) continue;

        //        auto entityB = static_cast<GameObject *>(contactManifold->getBody1()->getUserPointer());
        //        if (entityB == nullptr) continue;

        //        for (auto handler : _collisionHandlers)
        //            handler->handleCollision(entityA, entityB);
        //        _collisionHandlers->handleCollision(entityA, entityB);
        //        contactManifold->clearManifold();
    }
}

void PhysicsManager::AddObject(PhysicsObject *obj, short group, short mask)
{
    if (obj == nullptr)
    {
        return;
    }

    _dynamicsWorld->addRigidBody(obj->getRigidBody(), group, mask);
}

void PhysicsManager::RemoveObject(PhysicsObject *obj)
{
    if (obj == nullptr)
    {
        return;
    }

    _dynamicsWorld->removeCollisionObject(obj->getRigidBody());
}
