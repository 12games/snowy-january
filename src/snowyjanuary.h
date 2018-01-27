#ifndef SNOWYJANUARY_H
#define SNOWYJANUARY_H

#include "game.h"
#include "gl-color-normal-position-vertex.h"
#include "gl-masked-textures.h"
#include "physics.h"

class UpdatingTexture
{
    unsigned int _textureId;
    int _comp;
    glm::vec2 _planeSize;
    unsigned char *_pixels;

    void paintPixel(glm::vec2 const &at, std::vector<unsigned char> const &color);
public:
    UpdatingTexture();

    glm::vec2 _textureSize;
    unsigned int textureId() const;

    void loadTexture(std::string const &filename);
    void setPlaneSize(glm::vec2 const &planeSize);

    void paintOn(glm::vec2 const &pos, glm::vec2 const &dir);
};

class SnowyJanuary : public Game
{
    glm::mat4 _proj, _view;
    glm::vec3 _pos;

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

    unsigned int uploadTexture(std::string const &filename);

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
