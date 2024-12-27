// Dear ImGui: standalone example application for SDL3 + SDL_Renderer
// (SDL is a cross-platform general purpose library for handling windows, inputs, OpenGL/Vulkan/Metal graphics context creation, etc.)

// Learn about Dear ImGui:
// - FAQ                  https://dearimgui.com/faq
// - Getting Started      https://dearimgui.com/getting-started
// - Documentation        https://dearimgui.com/docs (same as your local docs/ folder).
// - Introduction, links and more at the top of imgui.cpp

// Important to understand: SDL_Renderer is an _optional_ component of SDL3.
// For a multi-platform app consider using e.g. SDL+DirectX on Windows and SDL+OpenGL on Linux/OSX.

#include "imgui.h"
#include "imgui_impl_sdl3.h"
#include "imgui_impl_sdlrenderer3.h"
#include <stdio.h>
#include <SDL3/SDL.h>
#if defined(IMGUI_IMPL_OPENGL_ES2)
#include <SDL3/SDL_opengles2.h>
#else
#include <SDL3/SDL_opengl.h>
#endif

#include "sdl_window.hpp"
#include "imgui_context.hpp"
#include "home_window.hpp"
#include "web_view.hpp"
#include "images.hpp"

// Main code
int main(int, char**)
{
    quicksorter::SdlWindowWrapper sdl {};
    quicksorter::ImGuiContext igc {sdl};

    // Our state
    bool show_demo_window = true;
    bool show_another_window = false;

    // Main loop
    bool done = false;
    quicksorter::HomeWindow win {sdl};
    while (!done)
    {
        // Poll and handle events (inputs, window resize, etc.)
        // You can read the io.WantCaptureMouse, io.WantCaptureKeyboard flags to tell if dear imgui wants to use your inputs.
        // - When io.WantCaptureMouse is true, do not dispatch mouse input data to your main application, or clear/overwrite your copy of the mouse data.
        // - When io.WantCaptureKeyboard is true, do not dispatch keyboard input data to your main application, or clear/overwrite your copy of the keyboard data.
        // Generally you may always pass all inputs to dear imgui, and hide them from your application based on those two flags.
        SDL_Event event;
        while (SDL_PollEvent(&event))
        {
            ImGui_ImplSDL3_ProcessEvent(&event);
            if (event.type == SDL_EVENT_QUIT)
                done = true;
            if (event.type == SDL_EVENT_WINDOW_CLOSE_REQUESTED && event.window.windowID == SDL_GetWindowID(sdl.window()))
                done = true;
        }
        if (SDL_GetWindowFlags(sdl.window()) & SDL_WINDOW_MINIMIZED)
        {
            SDL_Delay(10);
            continue;
        }

        {
          // Start the Dear ImGui frame
          auto frame {igc.MakeFrame()};

          // 1. Show the big demo window (Most of the sample code is in ImGui::ShowDemoWindow()! You can browse its code to learn more about Dear ImGui!).
          ImGui::ShowDemoWindow(&show_demo_window);

          // 2. Show a simple window that we create ourselves. We use a Begin/End pair to create a named window.
          {
              win.DisplayWindow();
          }
        }

        //SDL_RenderSetScale(sdl.renderer(), io.DisplayFramebufferScale.x, io.DisplayFramebufferScale.y);
        auto& clear_color = win.clear_color;
        SDL_SetRenderDrawColorFloat(sdl.renderer(), clear_color.x, clear_color.y, clear_color.z, clear_color.w);
        SDL_RenderClear(sdl.renderer());
        ImGui_ImplSDLRenderer3_RenderDrawData(ImGui::GetDrawData(), sdl.renderer());
        SDL_RenderPresent(sdl.renderer());
    }
#ifdef __EMSCRIPTEN__
    EMSCRIPTEN_MAINLOOP_END;
#endif

    return 0;
}
