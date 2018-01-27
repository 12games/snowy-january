#ifndef PHYSICSOBJECT_H
#define PHYSICSOBJECT_H

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>

class PhysicsObject
{
public:
    virtual ~PhysicsObject() {}

    virtual glm::mat4 const &getMatrix() const = 0;
    virtual class btRigidBody *getRigidBody() = 0;
};

class CarObject : public PhysicsObject
{
public:
    virtual void Update() = 0;
    virtual void StartEngine() = 0;
    virtual void ChangeSpeed(float amount) = 0;
    virtual void Steer(float amount) = 0;
    virtual void StopEngine() = 0;
};

class PhysicsObjectBuilder
{
    class PhysicsManager &_manager;
    class btCollisionShape* _shape;
    glm::vec3 _initialPos;
    glm::quat _initialRot;
    float _mass;
    float _friction;
    float _linearDamping;
    float _angularDamping;
    glm::vec3 _inputSize;

public:
    PhysicsObjectBuilder(class PhysicsManager &manager);
    PhysicsObjectBuilder& Box(glm::vec3 const &size);
    PhysicsObjectBuilder& Sphere(float radius);
    PhysicsObjectBuilder& Cylinder(glm::vec3 const &size);
    PhysicsObjectBuilder& Cone(float radius, float height);
    PhysicsObjectBuilder& Car(glm::vec3 const &size);

    PhysicsObjectBuilder& InitialPosition(glm::vec3 const &position);
    PhysicsObjectBuilder& InitialRotation(glm::quat const &rotation);
    PhysicsObjectBuilder& Mass(float amount);
    PhysicsObjectBuilder& Friction(float amount);
    PhysicsObjectBuilder& LinearDamping(float amount);
    PhysicsObjectBuilder& AngularDamping(float amount);

    PhysicsObject* Build();
    CarObject* BuildCar();
};

#endif // PHYSICSOBJECT_H
