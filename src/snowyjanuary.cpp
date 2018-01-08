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

bool SnowyJanuary::Setup()
{
    ImGuiIO &io = ImGui::GetIO();
    ImFont *font = io.Fonts->AddFontFromFileTTF("c:\\Windows\\Fonts\\tahoma.ttf", 18.0f, NULL);

    glClearColor(0.56f, 0.7f, 0.67f, 1.0f);

    // Setting up the shader
    _shader.compileDefaultShader();

    // Setting up the vertex buffer
    _floor
        .color(glm::vec4(0.0f, 1.0f, 1.0f, 1.0f))
        .vertex(glm::vec3(-10.0f, -10.0f, 0.0f)) // mint
        .color(glm::vec4(1.0f, 1.0f, 0.0f, 1.0f))
        .vertex(glm::vec3(-10.0f, 10.0f, 0.0f)) // geel
        .color(glm::vec4(1.0f, 0.0f, 1.0f, 1.0f))
        .vertex(glm::vec3(10.0f, 10.0f, 0.0f)) // paars
        .color(glm::vec4(1.0f, 1.0f, 1.0f, 1.0f))
        .vertex(glm::vec3(10.0f, -10.0f, 0.0f)) // wit
        .setup(GL_TRIANGLE_FAN);

    _floorObject = PhysicsObjectBuilder()
            .Box(glm::vec3(10.0f, 10.0f, 0.1f))
            .Mass(0.0f)
            .Build();
    _physics.AddObject(_floorObject);

    _box
        .color(glm::vec4(0.0f, 1.0f, 1.0f, 1.0f))
        .vertex(glm::vec3(-1.0f, -1.0f, 0.0f)) // mint
        .color(glm::vec4(1.0f, 1.0f, 0.0f, 1.0f))
        .vertex(glm::vec3(-1.0f, 1.0f, 0.0f)) // geel
        .color(glm::vec4(1.0f, 0.0f, 1.0f, 1.0f))
        .vertex(glm::vec3(1.0f, 1.0f, 0.0f)) // paars
        .color(glm::vec4(1.0f, 1.0f, 1.0f, 1.0f))
        .vertex(glm::vec3(1.0f, -1.0f, 0.0f)) // wit
        .setup(GL_TRIANGLE_FAN);

    _boxObject1 = PhysicsObjectBuilder()
            .Box(glm::vec3(1.0f, 1.0f, 1.0f))
            .Mass(1.0f)
            .InitialPosition(glm::vec3(0.0f, 0.0f, 10.0f))
            .Build();
    _physics.AddObject(_boxObject1);

    _boxObject2 = PhysicsObjectBuilder()
            .Box(glm::vec3(1.0f, 1.0f, 1.0f))
            .Mass(1.0f)
            .InitialPosition(glm::vec3(0.0f, 0.5f, 12.0f))
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

    _shader.setupMatrices(_proj, _view, _boxObject1->getMatrix());
    _box.render();
    _shader.setupMatrices(_proj, _view, _boxObject2->getMatrix());
    _box.render();
}

void SnowyJanuary::RenderUi()
{
    static float f = 0.0f;

    ImGui::Text("Hello, world!");                            // Some text (you can use a format string too)
    ImGui::SliderFloat("float", &f, 0.0f, 1.0f);             // Edit 1 float as a slider from 0.0f to 1.0f
    ImGui::ColorEdit3("clear color", (float *)&clear_color); // Edit 3 floats as a color
    ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
}

void SnowyJanuary::Destroy()
{
}
