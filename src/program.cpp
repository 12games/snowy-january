#include <glad/glad.h>
#include <iostream>

#define SDL_MAIN_HANDLED
#include <SDL2/SDL.h>
#include <imgui.h>
#include "imgui_impl_sdl_gl3.h"

#include "game.h"

#define TICK_INTERVAL 1000 / 60
#define WINDOW_WIDTH 640
#define WINDOW_HEIGHT 480

int main(int argc, char *argv[])
{
    SDL_Window *window;
    SDL_GLContext context;
    SDL_Event event;
    bool done = false;
    Uint32 lastUpdate = 0;
    Game &game = Game::Instantiate();

    SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER);

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

    while (!done)
    {
        if (SDL_GetTicks() - lastUpdate > TICK_INTERVAL)
        {
            // Run Update()
            game.Update(SDL_GetTicks() - lastUpdate);

            lastUpdate = SDL_GetTicks();
        }

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

    // Clean up
    SDL_Quit();

    return 0;
}
