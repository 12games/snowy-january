#include "physicsobject.h"
#include "physics.h"

#include <btBulletCollisionCommon.h>
#include <btBulletDynamicsCommon.h>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

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

class CarPhysicsObject : public CarObject, public ImplPhysicsObject
{
public:
    btRaycastVehicle *_vehicle;
    btDefaultVehicleRaycaster *_vehicleRayCaster;

    virtual void Update();
    virtual void Start();
    virtual void Stop();

    virtual glm::mat4 const &getMatrix() const;
    virtual class btRigidBody *getRigidBody();
};

void CarPhysicsObject::Update()
{
    _vehicle->applyEngineForce(100.0f, 3);
    _vehicle->applyEngineForce(100.0f, 2);

    _vehicle->setSteeringValue(0.3f, 1);
    _vehicle->setSteeringValue(0.3f, 0);
}

void CarPhysicsObject::Start()
{
}

void CarPhysicsObject::Stop()
{
}

glm::mat4 const &CarPhysicsObject::getMatrix() const
{
    return ImplPhysicsObject::getMatrix();
}

btRigidBody *CarPhysicsObject::getRigidBody()
{
    return ImplPhysicsObject::getRigidBody();
}

PhysicsObjectBuilder::PhysicsObjectBuilder(PhysicsManager &manager)
    : _manager(manager)
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

void addWheels(
    btVector3 const &halfExtents,
    btRaycastVehicle *vehicle,
    btRaycastVehicle::btVehicleTuning tuning)
{
    //The direction of the raycast, the btRaycastVehicle uses raycasts instead of simiulating the wheels with rigid bodies
    btVector3 wheelDirectionCS0(0, 0, -1);

    //The axis which the wheel rotates arround
    btVector3 wheelAxleCS(1, 0, 0);

    btScalar suspensionRestLength(0.6);

    btScalar wheelWidth(0.6f);

    btScalar wheelRadius(0.5f);

    btScalar connectionHeight(.2f);

    //All the wheel configuration assumes the vehicle is centered at the origin and a right handed coordinate system is used
    btVector3 wheelConnectionPoint(halfExtents.x() - wheelRadius, halfExtents.z() - wheelWidth, connectionHeight);

    //Adds the front wheels to the btRaycastVehicle by shifting the connection point
    vehicle->addWheel(wheelConnectionPoint * btVector3(1, 1, 1), wheelDirectionCS0, wheelAxleCS, suspensionRestLength, wheelRadius, tuning, true);
    vehicle->addWheel(wheelConnectionPoint * btVector3(-1, 1, 1), wheelDirectionCS0, wheelAxleCS, suspensionRestLength, wheelRadius, tuning, true);

    //Adds the rear wheels, notice that the last parameter value is false
    vehicle->addWheel(wheelConnectionPoint * btVector3(1, -1, 1), wheelDirectionCS0, wheelAxleCS, suspensionRestLength, wheelRadius, tuning, false);
    vehicle->addWheel(wheelConnectionPoint * btVector3(-1, -1, 1), wheelDirectionCS0, wheelAxleCS, suspensionRestLength, wheelRadius, tuning, false);

    btVector3 p2(wheelConnectionPoint);
    btVector3 p3(wheelConnectionPoint * btVector3(-1, 1, 1));
    btVector3 p4(wheelConnectionPoint * btVector3(1, -1, 1));
    btVector3 p5(wheelConnectionPoint * btVector3(-1, -1, 1));

    //Configures each wheel of our vehicle, setting its friction, damping compression, etc.
    for (int i = 0; i < vehicle->getNumWheels(); i++)
    {
        btWheelInfo &wheel = vehicle->getWheelInfo(i);
        wheel.m_suspensionStiffness = 50;
        wheel.m_wheelsDampingRelaxation = 1;
        wheel.m_wheelsDampingCompression = 0.8f;
        wheel.m_frictionSlip = 0.8f;
        wheel.m_rollInfluence = 1;
    }
}

CarObject *PhysicsObjectBuilder::BuildCar()
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

    auto obj = new CarPhysicsObject();
    obj->_matrix = glm::translate(glm::mat4(1.0f), _initialPos);

    auto rbInfo = btRigidBody::btRigidBodyConstructionInfo(_mass, obj, _shape, localInertia);
    obj->_rigidBody = new btRigidBody(rbInfo);
    obj->_rigidBody->setActivationState(DISABLE_DEACTIVATION);

    float wheelRadius = 0.5f;
    float wheelWidth = 0.4f;
    float connectionHeight = 1.2f;
    btVector3 wheelDir(0, -1, 0);
    btVector3 wheelAxle(-1, 0, 0);
    btScalar suspensionRestLength(0.6f);
    btRaycastVehicle::btVehicleTuning _tuning;

    obj->_vehicleRayCaster = new btDefaultVehicleRaycaster(_manager._dynamicsWorld);
    obj->_vehicle = new btRaycastVehicle(_tuning, reinterpret_cast<btRigidBody *>(obj->_rigidBody), obj->_vehicleRayCaster);

    // TODO Move this to the AddObject() function of PhycsManager?
    _manager._dynamicsWorld->addVehicle(obj->_vehicle);

    obj->_vehicle->setCoordinateSystem(0, 1, 2);

    addWheels(btVector3(_inputSize.x, _inputSize.y, _inputSize.z), obj->_vehicle, _tuning);

    return obj;
}

PhysicsObjectBuilder &PhysicsObjectBuilder::Box(glm::vec3 const &size)
{
    _inputSize = size;
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
    _inputSize = size;
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
