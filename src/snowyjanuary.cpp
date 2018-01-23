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

SnowyJanuary::SnowyJanuary()
    : _floor(_floorShader), _box(_boxShader), _car(_boxShader)
{
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

unsigned int SnowyJanuary::uploadTexture(std::string const &filename)
{
    unsigned int texture = 0;

    int x, y, comp;
    auto pixels = stbi_load(filename.c_str(), &x, &y, &comp, 3);
    if (pixels != nullptr)
    {
        glGenTextures(1, &texture);

        glBindTexture(GL_TEXTURE_2D, texture);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        glTexImage2D(GL_TEXTURE_2D, 0, comp == 4 ? GL_RGBA : GL_RGB, x, y, 0, comp == 4 ? GL_RGBA : GL_RGB, GL_UNSIGNED_BYTE, pixels);
        free(pixels);

        glBindTexture(GL_TEXTURE_2D, 0);
    }

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
    _maskTexture = uploadTexture("../01-snowy-january/assets/level.png");

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
                      .InitialPosition(glm::vec3(0.0f, 0.0f, 10.0f))
                      .Build();
    _physics.AddObject(_boxObject1);

    _boxObject2 = PhysicsObjectBuilder(_physics)
                      .Box(glm::vec3(2.0f))
                      .Mass(1.0f)
                      .InitialPosition(glm::vec3(0.0f, -1.2f, 15.0f))
                      .Build();
    _physics.AddObject(_boxObject2);

    _car.cubeTriangles()
        .scale(glm::vec3(2.0f, 1.0f, 1.0f))
        .fillColor(glm::vec4(0.0f, 0.3f, 0.5f, 1.0f))
        .setup();

    _carObject = PhysicsObjectBuilder(_physics)
                      .Box(glm::vec3(2.0f, 1.0f, 1.0f))
                      .Mass(1000.0f)
                      .InitialPosition(glm::vec3(8.0f, -1.2f, 2.0f))
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

void SnowyJanuary::Update(int dt)
{
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
        _floorShader.setupTextures(_grassTexture, _asphaltTexture, _snowTexture, _maskTexture);
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
        ImGui::SliderFloat("float", &f, 0.0f, 1.0f);             // Edit 1 float as a slider from 0.0f to 1.0f
        ImGui::ColorEdit3("clear color", (float *)&clear_color); // Edit 3 floats as a color
        ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);

        ImGui::Button("Go", ImVec2(88, 36));
        ImGui::End();
    }
}

void SnowyJanuary::Destroy()
{
}
