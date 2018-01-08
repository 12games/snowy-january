#include "physics.h"
#include <glm/gtc/type_ptr.hpp>
#include <iostream>
#include <vector>

using namespace std;

PhysicsManager::Config PhysicsManager::_config = {9.81f};

PhysicsManager::PhysicsManager()
{
    this->_broadphase = new btDbvtBroadphase();

    this->_collisionConfiguration = new btDefaultCollisionConfiguration();
    this->_dispatcher = new btCollisionDispatcher(this->_collisionConfiguration);

    this->_solver = new btSequentialImpulseConstraintSolver();

    this->_dynamicsWorld = new btDiscreteDynamicsWorld(this->_dispatcher, this->_broadphase, this->_solver, this->_collisionConfiguration);
    this->_dynamicsWorld->setGravity(btVector3(0, 0, -PhysicsManager::_config._gravity));
}

PhysicsManager::~PhysicsManager()
{
    if (this->_dynamicsWorld != 0)
        delete this->_dynamicsWorld;
    this->_dynamicsWorld = 0;

    if (this->_solver != 0)
        delete this->_solver;
    this->_solver = 0;

    if (this->_dispatcher != 0)
        delete this->_dispatcher;
    this->_dispatcher = 0;

    if (this->_collisionConfiguration != 0)
        delete this->_collisionConfiguration;
    this->_collisionConfiguration = 0;

    if (this->_broadphase != 0)
        delete this->_broadphase;
    this->_broadphase = 0;
}

void PhysicsManager::Step(float gameTime)
{
    this->_dynamicsWorld->stepSimulation(gameTime, 1);
    int numManifolds = this->_dynamicsWorld->getDispatcher()->getNumManifolds();

    for (int i = 0; i < numManifolds; i++)
    {
        btPersistentManifold *contactManifold = this->_dynamicsWorld->getDispatcher()->getManifoldByIndexInternal(i);

        if (contactManifold->getNumContacts() <= 0) continue;

        if (contactManifold->getBody0()->getUserPointer() == nullptr) continue;
        if (contactManifold->getBody1()->getUserPointer() == nullptr) continue;

//        auto entityA = static_cast<GameObject *>(contactManifold->getBody0()->getUserPointer());
//        if (entityA == nullptr) continue;

//        auto entityB = static_cast<GameObject *>(contactManifold->getBody1()->getUserPointer());
//        if (entityB == nullptr) continue;

//        for (auto handler : this->_collisionHandlers)
//            handler->handleCollision(entityA, entityB);
//        this->_collisionHandlers->handleCollision(entityA, entityB);
//        contactManifold->clearManifold();
    }
}

void PhysicsManager::AddObject(PhysicsObject *obj, short group, short mask)
{
    if (obj == nullptr)
    {
        return;
    }

    this->_dynamicsWorld->addRigidBody(obj->getRigidBody(), group, mask);
}

void PhysicsManager::RemoveObject(PhysicsObject *obj)
{
    if (obj == nullptr)
    {
        return;
    }

    this->_dynamicsWorld->removeCollisionObject(obj->getRigidBody());
}

///*
// * Object
// */
//PhysicsObject::PhysicsObject(GameObject *obj, float mass)
//    : _obj(obj), _shape(0), _rigidBody(0)
//{
//    if (obj != 0)
//    {
//        glm::quat q = obj->_initialOrientation;
//        glm::vec3 v = obj->_initialLocation;
//        this->_transform.setRotation(btQuaternion(q.x, q.y, q.z, q.w));
//        this->_transform.setOrigin(btVector3(v.x, v.y, v.z));

//        if (obj->_boundingType == GameObject::Box)
//        {
//            this->_shape = new btBoxShape(btVector3(obj->_boundingParameters[0], obj->_boundingParameters[1], obj->_boundingParameters[2]));
//        }
//        else if (obj->_boundingType == GameObject::Sphere)
//        {
//            this->_shape = new btSphereShape(obj->_boundingParameters[0]);
//        }
//        else if (obj->_boundingType == GameObject::Cylinder)
//        {
//            this->_shape = new btCylinderShape(btVector3(obj->_boundingParameters[0], obj->_boundingParameters[1], obj->_boundingParameters[2]));
//        }
//        else if (obj->_boundingType == GameObject::Tree)
//        {
//            this->_shape = new btConeShape(obj->_boundingParameters[0], obj->_boundingParameters[1]);
//        }
//        else if (obj->_boundingType == GameObject::Character)
//        {
//            this->_shape = new btBoxShape(btVector3(2, 2, 2));
//        }
//        else if (obj->_boundingType == GameObject::CarType)
//        {
//            btTransform localTrans;
//            localTrans.setIdentity();
//            localTrans.setOrigin(btVector3(0, obj->_boundingParameters[1] + 0.5f, 0));
//            btCollisionShape *chassis = new btBoxShape(btVector3(obj->_boundingParameters[0], obj->_boundingParameters[1], obj->_boundingParameters[2]));
//            btCompoundShape *shape = new btCompoundShape();
//            shape->addChildShape(localTrans, chassis);
//            btCollisionShape *shover = new btBoxShape(btVector3(obj->_boundingParameters[0] * 1.5f, obj->_boundingParameters[1], 1.0f));
//            localTrans.setIdentity();
//            localTrans.setOrigin(btVector3(0.0f, obj->_boundingParameters[1] + 0.5f, obj->_boundingParameters[2] + 1.0f));
//            shape->addChildShape(localTrans, shover);
//            this->_shape = shape;
//        }
//        obj->SetPhysics(this);

//        btVector3 localInertia(0, 0, 0);
//        if (mass != 0.0f)
//            this->_shape->calculateLocalInertia(mass, localInertia);

//        btRigidBody::btRigidBodyConstructionInfo rbInfo(mass, this, this->_shape, localInertia);
//        btRigidBody rigidBody ;//= new btRigidBody(rbInfo);

//        // Lage friction betekend veel weerstand, dus neemt de snelheid snel af.
//        this->_rigidBody->setFriction(0.1f);
//        rigidBody.setDamping(0.9f, 0.9f);
//        this->_rigidBody->setUserPointer(obj);
//    }
//}

//PhysicsObject::~PhysicsObject()
//{}

//glm::vec3 PhysicsObject::position() const
//{
//    return glm::vec3(this->_transform.getOrigin().x(),
//                     this->_transform.getOrigin().y(),
//                     this->_transform.getOrigin().z());
//}

//void PhysicsObject::getWorldTransform(btTransform &worldTrans) const
//{
//    worldTrans = this->_transform;
//}

//void PhysicsObject::setWorldTransform(const btTransform &worldTrans)
//{
//    this->_transform = worldTrans;
//}
