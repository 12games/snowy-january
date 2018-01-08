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

class PhysicsObjectBuilder
{
    class btCollisionShape* _shape;
    glm::vec3 _initialPos;
    glm::quat _initialRot;
    float _mass;
    float _friction;
    float _linearDamping;
    float _angularDamping;

public:
    PhysicsObjectBuilder();
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
};

#endif // PHYSICSOBJECT_H
