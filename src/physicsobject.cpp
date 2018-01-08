#include "physicsobject.h"
#include <btBulletCollisionCommon.h>
#include <btBulletDynamicsCommon.h>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>

class ImplPhysicsObject : public btMotionState, public PhysicsObject
{
public:
    glm::mat4 _matrix;
    btRigidBody *_rigidBody;

    void getWorldTransform(btTransform &worldTrans) const;
    void setWorldTransform(const btTransform &worldTrans);

    virtual glm::mat4 const &getMatrix() const;
    virtual class btRigidBody *getRigidBody();
};

void ImplPhysicsObject::getWorldTransform(btTransform &worldTrans) const
{
    worldTrans.setFromOpenGLMatrix(glm::value_ptr(_matrix));
}

void ImplPhysicsObject::setWorldTransform(const btTransform &worldTrans)
{
    worldTrans.getOpenGLMatrix(glm::value_ptr(_matrix));
}

glm::mat4 const &ImplPhysicsObject::getMatrix() const
{
    return _matrix;
}

btRigidBody *ImplPhysicsObject::getRigidBody()
{
    return _rigidBody;
}

PhysicsObjectBuilder::PhysicsObjectBuilder()
{
    _shape = nullptr;
    _initialPos = glm::vec3(0.0f);
    _mass = 0;
    _friction = 0.1f;
    _linearDamping = 0.9f;
    _angularDamping = 0.9f;
}

PhysicsObject *PhysicsObjectBuilder::Build()
{
    if (_shape == nullptr)
    {
        return nullptr;
    }

    btVector3 localInertia(0, 0, 0);
    if (_mass != 0.0f)
    {
        _shape->calculateLocalInertia(_mass, localInertia);
    }

    auto obj = new ImplPhysicsObject();
    obj->_matrix = glm::translate(glm::mat4(1.0f), _initialPos);

    auto rbInfo = btRigidBody::btRigidBodyConstructionInfo(_mass, obj, _shape, localInertia);
    obj->_rigidBody = new btRigidBody(rbInfo);

    obj->_rigidBody->setFriction(_friction);
    obj->_rigidBody->setDamping(_linearDamping, _angularDamping);

    return obj;
}

PhysicsObjectBuilder &PhysicsObjectBuilder::Box(glm::vec3 const &size)
{
    this->_shape = new btBoxShape(btVector3(size.x / 2.0f, size.y / 2.0f, size.z / 2.0f));

    return (*this);
}

PhysicsObjectBuilder &PhysicsObjectBuilder::Sphere(float radius)
{
    this->_shape = new btSphereShape(radius);

    return (*this);
}

PhysicsObjectBuilder &PhysicsObjectBuilder::Cylinder(glm::vec3 const &size)
{
    this->_shape = new btCylinderShape(btVector3(size.x / 2.0f, size.y / 2.0f, size.z / 2.0f));

    return (*this);
}

PhysicsObjectBuilder &PhysicsObjectBuilder::Cone(float radius, float height)
{
    this->_shape = new btConeShape(radius, height);

    return (*this);
}

PhysicsObjectBuilder &PhysicsObjectBuilder::Car(glm::vec3 const &size)
{
    btTransform localTrans;
    localTrans.setIdentity();
    localTrans.setOrigin(btVector3(0, size.y + 0.5f, 0));

    btCollisionShape *chassis = new btBoxShape(btVector3(size.x, size.y, size.z));
    btCompoundShape *shape = new btCompoundShape();
    shape->addChildShape(localTrans, chassis);

    btCollisionShape *shover = new btBoxShape(btVector3(size.x * 1.5f, size.y, 1.0f));
    localTrans.setIdentity();
    localTrans.setOrigin(btVector3(0.0f, size.y + 0.5f, size.z + 1.0f));
    shape->addChildShape(localTrans, shover);

    this->_shape = shape;

    return (*this);
}

PhysicsObjectBuilder &PhysicsObjectBuilder::InitialPosition(glm::vec3 const &position)
{
    _initialPos = position;

    return (*this);
}

PhysicsObjectBuilder &PhysicsObjectBuilder::InitialRotation(glm::quat const &rotation)
{
    _initialRot = rotation;

    return (*this);
}

PhysicsObjectBuilder &PhysicsObjectBuilder::Mass(float amount)
{
    _mass = amount;

    return (*this);
}

PhysicsObjectBuilder &PhysicsObjectBuilder::Friction(float amount)
{
    _friction = amount;

    return (*this);
}

PhysicsObjectBuilder &PhysicsObjectBuilder::LinearDamping(float amount)
{
    _linearDamping = amount;

    return (*this);
}

PhysicsObjectBuilder &PhysicsObjectBuilder::AngularDamping(float amount)
{
    _angularDamping = amount;

    return (*this);
}
