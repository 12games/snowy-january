#ifndef UPDATINGTEXTURE_H
#define UPDATINGTEXTURE_H

#include <glm/glm.hpp>
#include <string>
#include <vector>

class UpdatingTexture
{
public:
    UpdatingTexture();

    glm::vec2 _textureSize;

    uint32_t textureId() const;

    void loadTexture(
        std::string const &filename);

    void setPlaneSize(
        glm::vec2 const &planeSize);

    void paintOn(
        glm::mat4 const &modelMatrix);

    std::vector<glm::vec2> listBluePixels();

private:
    uint32_t _textureId = 0;
    int _comp = 0;
    glm::vec2 _planeSize;
    unsigned char *_pixels = nullptr;

    void paintPixel(
        glm::vec2 const &at,
        std::vector<unsigned char> const &color);

    void paintLine(
        glm::vec2 const &from,
        glm::vec2 const &to,
        std::vector<unsigned char> const &color);
};

#endif // UPDATINGTEXTURE_H
