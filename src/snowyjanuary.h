#ifndef SNOWYJANUARY_H
#define SNOWYJANUARY_H

#include "game.h"
#include "gl-color-normal-position-vertex.h"
#include "gl-masked-textures.h"
#include "physics.h"
#include "updatingtexture.h"

#include <string>

enum class MenuModes
{
    NoMenu,
    MainMenu,
    KeyMappingMenu,
};

class SnowyJanuary : public Game
{
    glm::mat4 _proj, _view;
    glm::vec3 _pos;

    std::string _settingsDir;
    MenuModes _menuMode;

    MaskedTexturesBuffer::ShaderType _floorShader;
    MaskedTexturesBuffer::BufferType _floor;
    ShaderType _boxShader;
    BufferType _box;
    BufferType _car;

    unsigned int _snowTexture;
    unsigned int _grassTexture;
    unsigned int _asphaltTexture;
    UpdatingTexture _maskTexture;

    PhysicsManager _physics;
    PhysicsObject *_floorObject;
    PhysicsObject *_boxObject1;
    PhysicsObject *_boxObject2;
    CarObject *_carObject;
    std::vector<PhysicsObject *> _treeObjects;

    unsigned int uploadTexture(std::string const &filename);

public:
    SnowyJanuary(int argc, char *argv[]);

    virtual bool Setup();
    virtual void Resize(int width, int height);
    virtual void Update(int dt);
    virtual void RenderUi();
    virtual void Render();
    virtual void Destroy();
};

#endif // SNOWYJANUARY_H
