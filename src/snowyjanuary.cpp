#include "snowyjanuary.h"
#include <capabilityguard.h>
#include <glad/glad.h>
#include <imgui.h>

#define SYSTEM_IO_FILEINFO_IMPLEMENTATION
#include <system.io.fileinfo.h>
#include <system.io.path.h>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#define KEYMAP_FILE "snowyjanuary.keymap"

Game &Game::Instantiate(int argc, char *argv[])
{
    static SnowyJanuary game(argc, argv);

    return game;
}

SnowyJanuary::SnowyJanuary(int argc, char *argv[])
    : _floor(_floorShader), _box(_boxShader), _car(_boxShader)
{
    System::IO::FileInfo exe(argv[0]);
    _settingsDir = exe.Directory().FullName();
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
    _userInput.ReadKeyMappings(System::IO::Path::Combine(_settingsDir, KEYMAP_FILE));

    glActiveTexture(GL_TEXTURE0);
    _snowTexture = uploadTexture("../01-snowy-january/assets/snow.bmp");
    glActiveTexture(GL_TEXTURE1);
    _grassTexture = uploadTexture("../01-snowy-january/assets/grass.bmp");
    glActiveTexture(GL_TEXTURE3);
    _asphaltTexture = uploadTexture("../01-snowy-january/assets/asphalt.bmp");
    glActiveTexture(GL_TEXTURE2);
    _maskTexture.loadTexture("../01-snowy-january/assets/level.png");
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
                      .InitialPosition(glm::vec3(5.0f, 2.2f, 5.0f))
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

    //    auto obj = PhysicsObjectBuilder(_physics)
    //                   .Cone(1.0f, 4.0f)
    //                   .Mass(0.0f)
    //                   .InitialPosition(glm::vec3(5.0f, 2.2f, 5.0f))
    //                   .InitialRotation(glm::quat(glm::vec3(glm::radians(90.0f), 0.0f, 0.0f)))
    //                   .Build();
    //    _physics.AddObject(obj);

    auto bluePixels = _maskTexture.listBluePixels();

    auto builder = PhysicsObjectBuilder(_physics)
                       .Cone(1.0f, 4.0f)
                       .Mass(0.0f)
                       .InitialRotation(glm::quat(glm::vec3(glm::radians(90.0f), 0.0f, 0.0f)));

    for (auto pos : bluePixels)
    {
        auto obj = builder
                       .InitialPosition(glm::vec3(pos.x, 2.2f, pos.y))
                       .Build();
        _physics.AddObject(obj);
        _treeObjects.push_back(obj);
    }

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
    if (_menuMode != MenuModes::NoMenu)
    {
        return;
    }

    if (_carObject->Speed() > 0)
    {
        _maskTexture.paintOn(_carObject->getMatrix());
    }

    _pos = glm::vec3(_carObject->getMatrix()[3].x, _carObject->getMatrix()[3].y, 0.0f);
    _view = glm::lookAt(_pos + glm::vec3(12.0f), _pos, glm::vec3(0.0f, 0.0f, 1.0f));

    if (_userInput.ActionState(UserInputActions::StartEngine))
    {
        _carObject->StartEngine();
    }
    else if (_userInput.ActionState(UserInputActions::StopEngine))
    {
        _carObject->StopEngine();
    }

    if (_userInput.ActionState(UserInputActions::SpeedUp))
    {
        _carObject->ChangeSpeed(1.0f);
    }
    else if (_userInput.ActionState(UserInputActions::SpeedDown))
    {
        _carObject->ChangeSpeed(-1.0f);
    }

    if (_userInput.ActionState(UserInputActions::SteerLeft))
    {
        _carObject->Steer(0.01f);
    }
    else if (_userInput.ActionState(UserInputActions::SteerRight))
    {
        _carObject->Steer(-0.01f);
    }

    if (_userInput.ActionState(UserInputActions::Brake))
    {
        _carObject->Brake();
    }

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
    static bool show_gui = true;

    if (_menuMode == MenuModes::NoMenu)
    {
        ImGui::Begin("Settings", &show_gui, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoSavedSettings);
        {
            ImGui::SetWindowPos(ImVec2(0, 0));
            ImGui::SetWindowSize(ImVec2(160, 64));
            if (ImGui::Button("Pause", ImVec2(120, 36)))
            {
                _menuMode = MenuModes::MainMenu;
            }
            ImGui::End();
        }
        return;
    }
    else
    {
        ImGui::Begin("Settings", &show_gui, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoSavedSettings);
        {
            float panelWidth = _width > 1024 ? 512 : 275;
            ImGui::SetWindowPos(ImVec2(0, 0));
            ImGui::SetWindowSize(ImVec2(panelWidth, _height));

            if (_menuMode == MenuModes::MainMenu)
            {
                if (ImGui::Button("Play!", ImVec2(100, 36)))
                {
                    _menuMode = MenuModes::NoMenu;
                }
                if (ImGui::Button("Change keys", ImVec2(100, 36)))
                {
                    _menuMode = MenuModes::KeyMappingMenu;
                }

                ImGui::Text("%.1f FPS", ImGui::GetIO().Framerate);
            }
            if (_menuMode == MenuModes::KeyMappingMenu)
            {
                if (ImGui::Button("Back", ImVec2(100, 36)))
                {
                    _menuMode = MenuModes::MainMenu;
                    _userInput.WriteKeyMappings(System::IO::Path::Combine(_settingsDir, KEYMAP_FILE));
                }
                ImGui::Columns(2);
                ImGui::SetColumnWidth(0, 120);

                ImGui::Text("Action");
                ImGui::NextColumn();
                ImGui::Text("Bound keys");
                ImGui::NextColumn();

                ImGui::Separator();

                for (int i = 0; i < int(UserInputActions::Count); ++i)
                {
                    if (ImGui::Button(UserInputActionNames[i], ImVec2(100, 36)))
                    {
                        _userInput.StartMappingAction((UserInputActions)i);
                    }
                    ImGui::NextColumn();

                    auto mappedEvents = _userInput.GetMappedActionEvents((UserInputActions)i);
                    bool first = true;
                    for (auto e : mappedEvents)
                    {
                        if (!first)
                        {
                            ImGui::Text("or");
                            ImGui::SameLine();
                        }
                        ImGui::Text(e.toString());
                        ImGui::SameLine();
                        first = false;
                    }
                    if (mappedEvents.size() == 0)
                    {
                        ImGui::Text("<unbound>");
                    }
                    ImGui::NextColumn();
                }
            }

            ImGui::End();
        }
    }
}

void SnowyJanuary::Destroy()
{
}
