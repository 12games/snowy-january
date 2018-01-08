#ifndef SNOWYJANUARY_H
#define SNOWYJANUARY_H

#include "game.h"
#include "gl-color-position-vertex.h"
#include "physics.h"

class SnowyJanuary : public Game
{
    glm::mat4 _proj, _view;
    glm::vec3 _pos;

    ShaderType _shader;
    BufferType _floor;
    BufferType _box;

    PhysicsManager _physics;
    PhysicsObject* _floorObject;
    PhysicsObject* _boxObject1;
    PhysicsObject* _boxObject2;

public:
    SnowyJanuary();

    virtual bool Setup();
    virtual void Resize(int width, int height);
    virtual void Update(int dt);
    virtual void RenderUi();
    virtual void Render();
    virtual void Destroy();
};

#endif // SNOWYJANUARY_H
