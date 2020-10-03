#include "physicsobject.h"
#include "physics.h"

#include <btBulletCollisionCommon.h>
#include <btBulletDynamicsCommon.h>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/quaternion.hpp>
#include <iostream>

class ImplPhysicsObject :
    public btMotionState,
    public PhysicsObject
{
public:
    glm::mat4 _matrix;
    btRigidBody *_rigidBody;

    void getWorldTransform(
        btTransform &worldTrans) const override;

    void setWorldTransform(
        const btTransform &worldTrans) override;

    virtual glm::mat4 const &getMatrix() const override;

    virtual class btRigidBody *getRigidBody() override;
};

void ImplPhysicsObject::getWorldTransform(
    btTransform &worldTrans) const
{
    worldTrans.setFromOpenGLMatrix(glm::value_ptr(_matrix));
}

void ImplPhysicsObject::setWorldTransform(
    const btTransform &worldTrans)
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

class CarPhysicsObject :
    public CarObject,
    public ImplPhysicsObject
{
public:
    CarPhysicsObject();

    void SetVehicle(
        btRaycastVehicle *vehicle,
        btDefaultVehicleRaycaster *vehicleRayCaster);

    virtual void Update() override;

    virtual bool EngineIstarted() override;

    virtual void StartEngine() override;

    virtual void ChangeSpeed(
        float amount) override;

    virtual void Steer(
        float amount) override;

    virtual void Brake() override;

    virtual void StopEngine() override;

    virtual float Speed() const override;

    virtual float Steering() const override;

    void setWorldTransform(
        const btTransform &worldTrans) override;

    virtual glm::mat4 const &getMatrix() const override;

    virtual class btRigidBody *getRigidBody() override;

    virtual glm::mat4 const &getWheelMatrix(
        int wheel) const override;

private:
    const float MIN_SPEED = -50.0f;
    const float MAX_SPEED = 100.0f;
    const float MIN_STEER = -0.3f;
    const float MAX_STEER = 0.3f;

    bool _engineStarted;
    float _speed;
    float _steering;
    bool _brakeNextUpdate;
    glm::mat4 _wheelMatrix[4];
    btRaycastVehicle *_vehicle;
    btDefaultVehicleRaycaster *_vehicleRayCaster;
};

CarPhysicsObject::CarPhysicsObject()
    : _engineStarted(false),
      _speed(0.0f),
      _steering(0.0f),
      _brakeNextUpdate(false),
      _vehicle(nullptr),
      _vehicleRayCaster(nullptr)
{
    _wheelMatrix[0] = glm::mat4(1.0f);
    _wheelMatrix[1] = glm::mat4(1.0f);
    _wheelMatrix[2] = glm::mat4(1.0f);
    _wheelMatrix[3] = glm::mat4(1.0f);
}

void CarPhysicsObject::setWorldTransform(
    const btTransform &worldTrans)
{
    for (int i = 0; i < 4; i++)
    {
        auto info = _vehicle->getWheelInfo(i);

        info.m_worldTransform.getOpenGLMatrix(glm::value_ptr(_wheelMatrix[i]));
    }

    ImplPhysicsObject::setWorldTransform(worldTrans);
}

void CarPhysicsObject::SetVehicle(
    btRaycastVehicle *vehicle,
    btDefaultVehicleRaycaster *vehicleRayCaster)
{
    _vehicle = vehicle;
    _vehicleRayCaster = vehicleRayCaster;
}

void CarPhysicsObject::Update()
{
    if (!_engineStarted)
    {
        return;
    }

    if (_brakeNextUpdate)
    {
        _vehicle->setBrake(100.0f, 3);
        _vehicle->setBrake(100.0f, 2);
        _speed = 0.0f;
        _brakeNextUpdate = false;
    }

    _vehicle->applyEngineForce(_speed, 2);
    _vehicle->applyEngineForce(_speed, 3);

    _vehicle->setSteeringValue(_steering, 1);
    _vehicle->setSteeringValue(_steering, 0);

    for (int i = 0; i < 4; i++)
    {
        auto info = _vehicle->getWheelInfo(i);

        info.m_worldTransform.getOpenGLMatrix(glm::value_ptr(_wheelMatrix[i]));
    }
}

bool CarPhysicsObject::EngineIstarted()
{
    return _engineStarted;
}

void CarPhysicsObject::StartEngine()
{
    if (!_engineStarted)
    {
        // todo: play sound of starting engine
        //       and update UI
    }

    _engineStarted = true;
}

void CarPhysicsObject::ChangeSpeed(
    float amount)
{
    if (!_engineStarted)
    {
        return;
    }

    _speed += amount;
    if (_speed > MAX_SPEED)
    {
        _speed = MAX_SPEED;
    }
    else if (_speed < MIN_SPEED)
    {
        _speed = MIN_SPEED;
    }
}

void CarPhysicsObject::Steer(
    float amount)
{
    if (!_engineStarted)
    {
        return;
    }

    _steering += amount;
    if (_steering > MAX_STEER)
    {
        _steering = MAX_STEER;
    }
    else if (_steering < MIN_STEER)
    {
        _steering = MIN_STEER;
    }
}

void CarPhysicsObject::Brake()
{
    _brakeNextUpdate = true;
}

void CarPhysicsObject::StopEngine()
{
    if (_engineStarted)
    {
        // todo play sound of stopping engine
        //      and update UI
    }

    _engineStarted = false;
    _speed = MIN_SPEED;
}

float CarPhysicsObject::Speed() const
{
    return _speed;
}

float CarPhysicsObject::Steering() const
{
    return _steering;
}

glm::mat4 const &CarPhysicsObject::getWheelMatrix(
    int wheel) const
{
    return _wheelMatrix[wheel];
}

glm::mat4 const &CarPhysicsObject::getMatrix() const
{
    return ImplPhysicsObject::getMatrix();
}

btRigidBody *CarPhysicsObject::getRigidBody()
{
    return ImplPhysicsObject::getRigidBody();
}

PhysicsObjectBuilder::PhysicsObjectBuilder(
    PhysicsManager &manager)
    : _manager(manager),
      _initialPos(glm::vec3(0.0f)),
      _initialRot(glm::toQuat(glm::mat4(1.0f)))
{
    _shape = nullptr;
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
    obj->_matrix = glm::toMat4(_initialRot) * glm::translate(glm::mat4(1.0f), _initialPos);

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

    btScalar suspensionRestLength(0.6f);

    btScalar wheelWidth(0.6f);

    btScalar wheelRadius(0.2f);

    btScalar connectionHeight(.2f);

    //All the wheel configuration assumes the vehicle is centered at the origin and a right handed coordinate system is used
    btVector3 wheelConnectionPoint(halfExtents.x() * 0.6f - wheelRadius, halfExtents.z() * 0.9f - wheelWidth, connectionHeight);

    //Adds the front wheels to the btRaycastVehicle by shifting the connection point
    vehicle->addWheel(wheelConnectionPoint * btVector3(1, 1, 1), wheelDirectionCS0, wheelAxleCS, suspensionRestLength, wheelRadius, tuning, true);
    vehicle->addWheel(wheelConnectionPoint * btVector3(-1, 1, 1), wheelDirectionCS0, wheelAxleCS, suspensionRestLength, wheelRadius, tuning, true);

    //Adds the rear wheels, notice that the last parameter value is false
    vehicle->addWheel(wheelConnectionPoint * btVector3(1, -1, 1), wheelDirectionCS0, wheelAxleCS, suspensionRestLength, wheelRadius, tuning, false);
    vehicle->addWheel(wheelConnectionPoint * btVector3(-1, -1, 1), wheelDirectionCS0, wheelAxleCS, suspensionRestLength, wheelRadius, tuning, false);

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

    btVector3 wheelDir(0, -1, 0);
    btVector3 wheelAxle(-1, 0, 0);
    btRaycastVehicle::btVehicleTuning _tuning;

    auto vehicleRayCaster = new btDefaultVehicleRaycaster(_manager._dynamicsWorld);
    auto vehicle = new btRaycastVehicle(_tuning, reinterpret_cast<btRigidBody *>(obj->_rigidBody), vehicleRayCaster);

    // TODO Move this to the AddObject() function of PhycsManager?
    _manager._dynamicsWorld->addVehicle(vehicle);

    vehicle->setCoordinateSystem(0, 1, 2);

    addWheels(btVector3(_inputSize.x, _inputSize.y, _inputSize.z), vehicle, _tuning);

    obj->SetVehicle(vehicle, vehicleRayCaster);

    return obj;
}

PhysicsObjectBuilder &PhysicsObjectBuilder::Box(
    glm::vec3 const &size)
{
    _inputSize = size;
    this->_shape = new btBoxShape(btVector3(size.x / 2.0f, size.y / 2.0f, size.z / 2.0f));

    return (*this);
}

PhysicsObjectBuilder &PhysicsObjectBuilder::Sphere(
    float radius)
{
    this->_shape = new btSphereShape(radius);

    return (*this);
}

PhysicsObjectBuilder &PhysicsObjectBuilder::Cylinder(
    glm::vec3 const &size)
{
    _inputSize = size;
    this->_shape = new btCylinderShape(btVector3(size.x / 2.0f, size.y / 2.0f, size.z / 2.0f));

    return (*this);
}

PhysicsObjectBuilder &PhysicsObjectBuilder::Cone(
    float radius,
    float height)
{
    this->_shape = new btConeShape(radius, height);

    return (*this);
}

PhysicsObjectBuilder &PhysicsObjectBuilder::Car(
    glm::vec3 const &size)
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
    localTrans.setRotation(btQuaternion(_initialRot.x, _initialRot.y, _initialRot.z, _initialRot.w));
    shape->addChildShape(localTrans, shover);

    this->_shape = shape;

    return (*this);
}

PhysicsObjectBuilder &PhysicsObjectBuilder::InitialPosition(
    glm::vec3 const &position)
{
    _initialPos = position;

    return (*this);
}

PhysicsObjectBuilder &PhysicsObjectBuilder::InitialRotation(
    glm::quat const &rotation)
{
    _initialRot = rotation;

    return (*this);
}

PhysicsObjectBuilder &PhysicsObjectBuilder::Mass(
    float amount)
{
    _mass = amount;

    return (*this);
}

PhysicsObjectBuilder &PhysicsObjectBuilder::Friction(
    float amount)
{
    _friction = amount;

    return (*this);
}

PhysicsObjectBuilder &PhysicsObjectBuilder::LinearDamping(
    float amount)
{
    _linearDamping = amount;

    return (*this);
}

PhysicsObjectBuilder &PhysicsObjectBuilder::AngularDamping(
    float amount)
{
    _angularDamping = amount;

    return (*this);
}
