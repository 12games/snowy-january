#include "snowyjanuary.h"
#include <glad/glad.h>
#include <imgui.h>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

Game &Game::Instantiate()
{
    static SnowyJanuary game;

    return game;
}

class CapabilityGuard
{
    GLenum _cap;
    short _prevValue;

public:
    CapabilityGuard(GLenum cap, bool enable)
        : _cap(cap)
    {
        _prevValue = glIsEnabled(cap) ? 1 : 0;
        if (enable != (_prevValue == 1))
        {
            if (enable)
            {
                glEnable(_cap);
            }
            else
            {
                glDisable(_cap);
            }
        }
        else
        {
            _prevValue = -1;
        }
    }
    virtual ~CapabilityGuard()
    {
        if (_prevValue == 1)
        {
            glEnable(_cap);
        }
        else if (_prevValue == 0)
        {
            glDisable(_cap);
        }
    }
};

UpdatingTexture::UpdatingTexture()
    : _pixels(nullptr)
{
}

unsigned int UpdatingTexture::textureId() const
{
    return _textureId;
}

void UpdatingTexture::loadTexture(std::string const &filename)
{
    int x, y;
    _pixels = stbi_load(filename.c_str(), &x, &y, &_comp, 3);
    if (_pixels == nullptr)
    {
        return;
    }

    _textureSize = glm::vec2(x, y);

    glGenTextures(1, &_textureId);

    glBindTexture(GL_TEXTURE_2D, _textureId);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glTexImage2D(GL_TEXTURE_2D, 0, _comp == 4 ? GL_RGBA : GL_RGB, _textureSize.x, _textureSize.y, 0, _comp == 4 ? GL_RGBA : GL_RGB, GL_UNSIGNED_BYTE, _pixels);

    glBindTexture(GL_TEXTURE_2D, 0);
}

void UpdatingTexture::setPlaneSize(glm::vec2 const &planeSize)
{
    _planeSize = planeSize;
}

void UpdatingTexture::paintPixel(glm::vec2 const &at, std::vector<unsigned char> const &color)
{
    if (at.x < 0 || at.x > _textureSize.x)
    {
        return;
    }
    if (at.y < 0 || at.y > _textureSize.y)
    {
        return;
    }

    auto pixelOffset = int((at.y * _textureSize.x) + at.x) * _comp;

    for (int i = 0; i < color.size(); i++)
    {
        _pixels[pixelOffset + i] = color[i];
    }
}

void UpdatingTexture::paintLine(glm::vec2 const &from, glm::vec2 const &to, std::vector<unsigned char> const &color)
{
    float x1 = from.x;
    float y1 = from.y;
    float x2 = to.x;
    float y2 = to.y;
    {
            // Bresenham's line algorithm
      const bool steep = (fabs(y2 - y1) > fabs(x2 - x1));
      if(steep)
      {
        std::swap(x1, y1);
        std::swap(x2, y2);
      }

      if(x1 > x2)
      {
        std::swap(x1, x2);
        std::swap(y1, y2);
      }

      const float dx = x2 - x1;
      const float dy = fabs(y2 - y1);

      float error = dx / 2.0f;
      const int ystep = (y1 < y2) ? 1 : -1;
      int y = (int)y1;

      const int maxX = (int)x2;

      for(int x=(int)x1; x<maxX; x++)
      {
        if(steep)
        {
            paintPixel(glm::vec2(y,x), color);
        }
        else
        {
            paintPixel(glm::vec2(x,y), color);
        }

        error -= dy;
        if(error < 0)
        {
            y += ystep;
            error += dx;
        }
      }
    }
}

void UpdatingTexture::paintOn(glm::vec2 const &pos, glm::mat4 const &modelMatrix)
{
    if (_pixels == nullptr)
    {
        return;
    }

    glm::vec2 dir = glm::vec2(modelMatrix[1].x, modelMatrix[1].y);
    glm::vec2 right = glm::vec2(modelMatrix[0].x, modelMatrix[0].y);

    // three lines to make sure the most pixels are painted over even when the car is moving fast or making  sharp turn
    auto localPos = pos + (dir * 30.0f);
    paintLine(localPos + (right * 20.0f), localPos + (right * -20.0f), std::vector<unsigned char>({ 0, 255, 0, 0}));
    localPos = pos + (dir * 29.0f);
    paintLine(localPos + (right * 20.0f), localPos + (right * -20.0f), std::vector<unsigned char>({ 0, 255, 0, 0}));
    localPos = pos + (dir * 28.0f);
    paintLine(localPos + (right * 20.0f), localPos + (right * -20.0f), std::vector<unsigned char>({ 0, 255, 0, 0}));

    glTexImage2D(GL_TEXTURE_2D, 0, _comp == 4 ? GL_RGBA : GL_RGB, _textureSize.x, _textureSize.y, 0, _comp == 4 ? GL_RGBA : GL_RGB, GL_UNSIGNED_BYTE, _pixels);
}




SnowyJanuary::SnowyJanuary()
    : _floor(_floorShader), _box(_boxShader), _car(_boxShader)
{
}

unsigned int SnowyJanuary::uploadTexture(std::string const &filename)
{
    int x, y, comp;
    auto pixels = stbi_load(filename.c_str(), &x, &y, &comp, 3);
    if (pixels == nullptr)
    {
        return 0;
    }

    unsigned int texture = 0;

    glGenTextures(1, &texture);

    glBindTexture(GL_TEXTURE_2D, texture);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glTexImage2D(GL_TEXTURE_2D, 0, comp == 4 ? GL_RGBA : GL_RGB, x, y, 0, comp == 4 ? GL_RGBA : GL_RGB, GL_UNSIGNED_BYTE, pixels);
    free(pixels);

    glBindTexture(GL_TEXTURE_2D, 0);

    return texture;
}

bool SnowyJanuary::Setup()
{
    glActiveTexture(GL_TEXTURE0);
    _snowTexture = uploadTexture("../01-snowy-january/assets/snow.bmp");
    glActiveTexture(GL_TEXTURE1);
    _grassTexture = uploadTexture("../01-snowy-january/assets/grass.bmp");
    glActiveTexture(GL_TEXTURE3);
    _asphaltTexture = uploadTexture("../01-snowy-january/assets/asphalt.bmp");
    glActiveTexture(GL_TEXTURE2);
    _maskTexture.loadTexture("../01-snowy-january/assets/level-clear.png");
    _maskTexture.setPlaneSize(glm::vec2(20.0f, 20.0f));

    ImGuiIO &io = ImGui::GetIO();
    ImFont *font = io.Fonts->AddFontFromFileTTF("c:\\Windows\\Fonts\\tahoma.ttf", 18.0f, NULL);

    ImGuiStyle &style = ImGui::GetStyle();
    style.WindowRounding = 0.0f;
    style.WindowPadding = ImVec2(15.0f, 15.0f);
    style.ItemSpacing = ImVec2(15.0f, 15.0f);
    style.ItemInnerSpacing = ImVec2(16.0f, 8.0f);
    style.Colors[ImGuiCol_WindowBg] = ImVec4(0.0f, 0.0f, 0.0f, 0.1f);
    style.Colors[ImGuiCol_Text] = ImVec4(0.1f, 0.2f, 0.2f, 1.0f);

    glClearColor(0.56f, 0.7f, 0.67f, 1.0f);

    // Setting up the shaders
    _floorShader.compileDefaultShader();
    _boxShader.compileDefaultShader();

    // Setting up the vertex buffer
    _floor.planeTriangleFan(glm::vec2(20.0f), glm::vec2(5.12f))
        .setup();

    _floorObject = PhysicsObjectBuilder(_physics)
                       .Box(glm::vec3(20.0f, 20.0f, 0.1f))
                       .Mass(0.0f)
                       .Build();
    _physics.AddObject(_floorObject);

    _box.cubeTriangles()
        .scale(glm::vec3(2.0f))
        .fillColor(glm::vec4(0.0f, 0.3f, 0.5f, 1.0f))
        .setup();

    _boxObject1 = PhysicsObjectBuilder(_physics)
                      .Box(glm::vec3(2.0f))
                      .Mass(1.0f)
                      .InitialPosition(glm::vec3(8.0f, 2.2f, 2.0f))
                      .Build();
    _physics.AddObject(_boxObject1);

    _boxObject2 = PhysicsObjectBuilder(_physics)
                      .Box(glm::vec3(2.0f))
                      .Mass(1.0f)
                      .InitialPosition(glm::vec3(8.0f, -1.2f, 2.0f))
                      .Build();
    _physics.AddObject(_boxObject2);

    _car.cubeTriangles()
        .scale(glm::vec3(1.0f, 2.0f, 1.0f))
        .fillColor(glm::vec4(0.0f, 0.3f, 0.5f, 1.0f))
        .setup();

    _carObject = PhysicsObjectBuilder(_physics)
                     .Box(glm::vec3(1.0f, 2.0f, 1.0f))
                     .Mass(1000.0f)
                     .InitialPosition(glm::vec3(0.0f, 0.0f, 2.0f))
                     .BuildCar();
    _physics.AddObject(_carObject);

    _physics.InitDebugDraw();

    return true;
}

void SnowyJanuary::Resize(int width, int height)
{
    _width = width;
    _height = height;

    // Calculate the projection and view matrix
    _proj = glm::perspective(glm::radians(90.0f), float(width) / float(height), 0.1f, 4096.0f);
    _view = glm::lookAt(_pos + glm::vec3(12.0f), _pos, glm::vec3(0.0f, 0.0f, 1.0f));
}

static int px = 0;
static int py = 502;

void SnowyJanuary::Update(int dt)
{
    auto mat = _carObject->getMatrix();
    px = int((mat[3].x + 10.0f)*25.6f);
    py = int((mat[3].y + 10.0f)*25.6f);

    _maskTexture.paintOn(glm::vec2(px, py), mat);
    _carObject->Update();
    _physics.Step(dt / 1000.0f);
}

static ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

void SnowyJanuary::Render()
{
    glViewport(0, 0, _width, _height);

    glClearColor(clear_color.x, clear_color.y, clear_color.z, clear_color.w);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // Clear Screen And Depth Buffer

    // Select shader
    _floorShader.use();

    {
        CapabilityGuard texture2d(GL_TEXTURE_2D, true);

        _floorShader.setupMatrices(_proj, _view, _floorObject->getMatrix());
        _floorShader.setupTextures(_grassTexture, _asphaltTexture, _snowTexture, _maskTexture.textureId());
        _floor.render();
    }

    {
        CapabilityGuard cullFace(GL_CULL_FACE, true);
        CapabilityGuard depthTest(GL_DEPTH_TEST, true);

        // Select shader
        _boxShader.use();

        _boxShader.setupMatrices(_proj, _view, _boxObject1->getMatrix());
        _box.render();
        _boxShader.setupMatrices(_proj, _view, _boxObject2->getMatrix());
        _box.render();

        _boxShader.setupMatrices(_proj, _view, _carObject->getMatrix());
        _car.render();
    }
    CapabilityGuard depthTest(GL_DEPTH_TEST, false);
    _physics.DebugDraw(_proj, _view);
}

void SnowyJanuary::RenderUi()
{
    static float f = 0.0f;
    static bool show_gui = true;

    ImGui::Begin("Settings", &show_gui, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoSavedSettings);
    {
        ImGui::SetWindowPos(ImVec2(0, 0));
        ImGui::SetWindowSize(ImVec2(_width > 1024 ? 550 : 275, _height));

        ImGui::Text("Hello, world!");                            // Some text (you can use a format string too)
        ImGui::SliderInt("x", &(px), 0, 512.0f);
        ImGui::SliderInt("y", &(py), 0, 512.0f);
        ImGui::ColorEdit3("clear color", (float *)&clear_color); // Edit 3 floats as a color
        ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);

        ImGui::Button("Go", ImVec2(88, 36));
        ImGui::End();
    }
}

void SnowyJanuary::Destroy()
{
}
