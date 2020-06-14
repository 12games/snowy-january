#include <glad/glad.h>
#include <iostream>

#define SDL_MAIN_HANDLED
#include <SDL2/SDL.h>

#include <imgui.h>

#include "imgui_impl_sdl_gl3.h"

#include "audio.h"
#include "game.h"

#define TINYOBJLOADER_IMPLEMENTATION
#include <tiny_obj_loader.h>

#define TICK_INTERVAL 1000 / 120
#define WINDOW_WIDTH 1024
#define WINDOW_HEIGHT 768

int main(int argc, char *argv[])
{
    SDL_Window *window;
    SDL_GLContext context;
    SDL_Event event;
    bool done = false;
    Uint32 lastUpdate = 0;
    Game &game = Game::Instantiate(argc, argv);

    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_GAMECONTROLLER | SDL_INIT_TIMER) < 0)
    {
        return 1;
    }

    initAudio();

    SDL_Delay(1000);

    SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, SDL_GL_CONTEXT_FORWARD_COMPATIBLE_FLAG);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
    SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 2);

    window = SDL_CreateWindow(
        "Snowy January",
        SDL_WINDOWPOS_UNDEFINED,
        SDL_WINDOWPOS_UNDEFINED,
        WINDOW_WIDTH,
        WINDOW_HEIGHT,
        SDL_WINDOW_RESIZABLE | SDL_WINDOW_MAXIMIZED | SDL_WINDOW_OPENGL);

    // Check that the window was successfully created
    if (window == NULL)
    {
        // In the case that the window could not be made...
        std::cerr << "Could not create window: " << SDL_GetError() << std::endl;
        return 1;
    }

    context = SDL_GL_CreateContext(window);
    if (context == 0)
    {
        std::cerr << "Unable to create GL context: %s\n"
                  << SDL_GetError() << std::endl;
        return 2;
    }

    if (!gladLoadGL())
    {
        std::cerr << "Something went wrong loading GL!\n"
                  << std::endl;
        return 3;
    }

    ImGui_ImplSdlGL3_Init(window);

    // Run Setup()
    if (!game.Setup())
    {
        std::cerr << "Game.Setup() failed!" << std::endl;
        return 4;
    }

    game.Resize(WINDOW_WIDTH, WINDOW_HEIGHT);

    if (SDL_IsGameController(0))
    {
        std::cout << "is controlle" << std::endl;
    }

    SDL_GameControllerEventState(SDL_ENABLE);
    SDL_GameControllerOpen(0);

    while (!done)
    {
        while (SDL_PollEvent(&event))
        {
            ImGui_ImplSdlGL3_ProcessEvent(&event);

            if (event.type == SDL_QUIT || event.type == SDL_WINDOWEVENT_CLOSE)
            {
                done = 1;
            }
            if (event.type == SDL_WINDOWEVENT && event.window.event == SDL_WINDOWEVENT_SIZE_CHANGED)
            {
                game.Resize(event.window.data1, event.window.data2);
            }
            if (event.type == SDL_KEYDOWN || event.type == SDL_KEYUP)
            {
                UserInputMapping uie = {
                    SDL_KEYDOWN,
                    0,
                    event.key.keysym.sym,
                    event.type == SDL_KEYDOWN ? 0 : 255,
                };
                game._userInput.ProcessEvent(uie, (event.type == SDL_KEYDOWN));
            }
            if (event.type == SDL_CONTROLLERBUTTONUP || event.type == SDL_CONTROLLERBUTTONDOWN)
            {
                std::cout << "button: " << int(event.cbutton.button) << std::endl
                          << "type: " << (event.type == SDL_CONTROLLERBUTTONUP ? "UP" : "DOWN") << std::endl;

                UserInputMapping uie = {
                    SDL_CONTROLLERBUTTONDOWN,
                    event.cbutton.which,
                    event.cbutton.button,
                    event.type == SDL_CONTROLLERBUTTONDOWN ? 0 : 255,
                };
                game._userInput.ProcessEvent(uie, (event.type == SDL_CONTROLLERBUTTONDOWN));
            }
            if (event.type == SDL_CONTROLLERAXISMOTION)
            {
                std::cout << "axis: " << int(event.caxis.axis) << std::endl
                          << "value: " << int(event.caxis.value) << std::endl
                          << "type: " << int(event.type) << std::endl;

                UserInputMapping uie = {
                    SDL_CONTROLLERAXISMOTION,
                    event.caxis.which,
                    event.caxis.axis,
                    event.caxis.value,
                };
                game._userInput.ProcessEvent(uie, (event.type == SDL_CONTROLLERBUTTONDOWN));
            }
        }

        if (SDL_GetTicks() - lastUpdate > TICK_INTERVAL)
        {
            game._userInput.StartUsingQueuedEvents();

            // Run Update()
            game.Update(SDL_GetTicks() - lastUpdate);

            lastUpdate = SDL_GetTicks();

            game._userInput.EndUsingQueuedEvents();
        }

        ImGui_ImplSdlGL3_NewFrame(window);

        // Run Render()
        game.Render();

        game.RenderUi();

        ImGui::Render();

        /* Swap our back buffer to the front */
        SDL_GL_SwapWindow(window);
    }

    // Run Destroy()
    game.Destroy();

    ImGui_ImplSdlGL3_Shutdown();

    SDL_GL_DeleteContext(context);

    // Close and destroy the window
    SDL_DestroyWindow(window);

    endAudio();

    // Clean up
    SDL_Quit();

    return 0;
}

char const *UserInputMapping::toString()
{
    if (source == SDL_KEYDOWN)
    {
        switch (key)
        {
            case SDLK_RETURN:
                return "Enter";
            case SDLK_ESCAPE:
                return "Escape";
            case SDLK_BACKSPACE:
                return "Backspace";
            case SDLK_TAB:
                return "Tab";
            case SDLK_SPACE:
                return "SPACE";
            case SDLK_EXCLAIM:
                return "!";
            case SDLK_QUOTEDBL:
                return "\"";
            case SDLK_HASH:
                return "#";
            case SDLK_PERCENT:
                return "%";
            case SDLK_DOLLAR:
                return "$";
            case SDLK_AMPERSAND:
                return "&";
            case SDLK_QUOTE:
                return "\'";
            case SDLK_LEFTPAREN:
                return "(";
            case SDLK_RIGHTPAREN:
                return ")";
            case SDLK_ASTERISK:
                return "*";
            case SDLK_PLUS:
                return "+";
            case SDLK_COMMA:
                return ",";
            case SDLK_MINUS:
                return "-";
            case SDLK_PERIOD:
                return ".";
            case SDLK_SLASH:
                return "/";
            case SDLK_0:
                return "0";
            case SDLK_1:
                return "1";
            case SDLK_2:
                return "2";
            case SDLK_3:
                return "3";
            case SDLK_4:
                return "4";
            case SDLK_5:
                return "5";
            case SDLK_6:
                return "6";
            case SDLK_7:
                return "7";
            case SDLK_8:
                return "8";
            case SDLK_9:
                return "9";
            case SDLK_COLON:
                return ":";
            case SDLK_SEMICOLON:
                return ";";
            case SDLK_LESS:
                return "<";
            case SDLK_EQUALS:
                return "=";
            case SDLK_GREATER:
                return ">";
            case SDLK_QUESTION:
                return "?";
            case SDLK_AT:
                return "@";
            case SDLK_LEFTBRACKET:
                return "[";
            case SDLK_BACKSLASH:
                return "\\";
            case SDLK_RIGHTBRACKET:
                return "]";
            case SDLK_CARET:
                return "^";
            case SDLK_UNDERSCORE:
                return "_";
            case SDLK_BACKQUOTE:
                return "`";
            case SDLK_a:
                return "a";
            case SDLK_b:
                return "b";
            case SDLK_c:
                return "c";
            case SDLK_d:
                return "d";
            case SDLK_e:
                return "e";
            case SDLK_f:
                return "f";
            case SDLK_g:
                return "g";
            case SDLK_h:
                return "h";
            case SDLK_i:
                return "i";
            case SDLK_j:
                return "j";
            case SDLK_k:
                return "k";
            case SDLK_l:
                return "l";
            case SDLK_m:
                return "m";
            case SDLK_n:
                return "n";
            case SDLK_o:
                return "o";
            case SDLK_p:
                return "p";
            case SDLK_q:
                return "q";
            case SDLK_r:
                return "r";
            case SDLK_s:
                return "s";
            case SDLK_t:
                return "t";
            case SDLK_u:
                return "u";
            case SDLK_v:
                return "v";
            case SDLK_w:
                return "w";
            case SDLK_x:
                return "x";
            case SDLK_y:
                return "y";
            case SDLK_z:
                return "z";
            case SDLK_F1:
                return "F1";
            case SDLK_F2:
                return "F2";
            case SDLK_F3:
                return "F3";
            case SDLK_F4:
                return "F4";
            case SDLK_F5:
                return "F5";
            case SDLK_F6:
                return "F6";
            case SDLK_F7:
                return "F7";
            case SDLK_F8:
                return "F8";
            case SDLK_F9:
                return "F9";
            case SDLK_F10:
                return "F10";
            case SDLK_F11:
                return "F11";
            case SDLK_F12:
                return "F12";

            case SDLK_LCTRL:
                return "Left CTRL";
            case SDLK_RCTRL:
                return "Right CTRL";
            case SDLK_LSHIFT:
                return "Left SHIFT";
            case SDLK_RSHIFT:
                return "Right SHIFT";
            case SDLK_LALT:
                return "Left ALT";
            case SDLK_RALT:
                return "Right ALT";

            case SDLK_LEFT:
                return "Left Arrow";
            case SDLK_RIGHT:
                return "Right Arrow";
            case SDLK_UP:
                return "Up Arrow";
            case SDLK_DOWN:
                return "Down Arrow";
        }
    }
    else if (SDL_CONTROLLERBUTTONDOWN)
    {
        switch (key)
        {
            case 0:
                return "Controller button 1";
            case 1:
                return "Controller button 2";
            case 2:
                return "Controller button 3";
            case 3:
                return "Controller button 4";
            case 4:
                return "Controller button 5";
            case 5:
                return "Controller button 6";
            case 6:
                return "Controller button 7";
            case 7:
                return "Controller button 8";
            case 8:
                return "Controller button 9";
            case 9:
                return "Controller button 10";
            case 10:
                return "Controller button 11";
            case 11:
                return "Controller button 12";
            case 12:
                return "Controller button 13";
            case 13:
                return "Controller button 14";
            case 14:
                return "Controller button 15";
            case 15:
                return "Controller button 16";
            case 16:
                return "Controller button 17";
        }
        if (player == 0)
        {
            return "Controller player 1";
        }
        if (player == 1)
        {
            return "Controller player 2";
        }
    }
    else if (SDL_CONTROLLERAXISMOTION)
    {
        if (key == 0 && value > 0)
            return "Controller Up";
        else if (key == 0 && value < 0)
            return "Controller Down";
        else if (key == 1 && value > 0)
            return "Controller Left";
        else if (key == 1 && value < 0)
            return "Controller Right";
    }
    return "<unknown>";
}
