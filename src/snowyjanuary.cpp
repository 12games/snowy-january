#include "snowyjanuary.h"
#include <glad/glad.h>
#include <imgui.h>

Game &Game::Instantiate()
{
    static SnowyJanuary game;

    return game;
}

SnowyJanuary::SnowyJanuary()
    : _floor(_shader), _box(_shader)
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

bool SnowyJanuary::Setup()
{
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

    // Setting up the shader
    _shader.compileDefaultShader();

    // Setting up the vertex buffer
    _floor.planeTriangleFan()
        .scale((glm::vec3(20.0f)))
        .setup();

    _floorObject = PhysicsObjectBuilder()
                       .Box(glm::vec3(20.0f, 20.0f, 0.1f))
                       .Mass(0.0f)
                       .Build();
    _physics.AddObject(_floorObject);

    _box.cubeTriangles()
        .scale(glm::vec3(2.0f))
            .fillColor(glm::vec4(0.0f, 0.3f, 0.5f, 1.0f))
        .setup();

    _boxObject1 = PhysicsObjectBuilder()
                      .Box(glm::vec3(2.0f))
                      .Mass(1.0f)
                      .InitialPosition(glm::vec3(0.0f, 0.0f, 10.0f))
                      .Build();
    _physics.AddObject(_boxObject1);

    _boxObject2 = PhysicsObjectBuilder()
                      .Box(glm::vec3(2.0f))
                      .Mass(1.0f)
                      .InitialPosition(glm::vec3(0.0f, -1.2f, 15.0f))
                      .Build();
    _physics.AddObject(_boxObject2);

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
    _physics.Step(dt / 1000.0f);
}

static ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

void SnowyJanuary::Render()
{
    glViewport(0, 0, _width, _height);

    glClearColor(clear_color.x, clear_color.y, clear_color.z, clear_color.w);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // Clear Screen And Depth Buffer

    // Select shader
    _shader.use();

    _shader.setupMatrices(_proj, _view, _floorObject->getMatrix());
    _floor.render();

    CapabilityGuard cullFace(GL_CULL_FACE, true);
    CapabilityGuard depthTest(GL_DEPTH_TEST, true);

    _shader.setupMatrices(_proj, _view, _boxObject1->getMatrix());
    _box.render();
    _shader.setupMatrices(_proj, _view, _boxObject2->getMatrix());
    _box.render();
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
