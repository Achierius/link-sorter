#include "home_window.hpp"

#include "images.hpp"
#include "imgui.h"
#include "imgui_impl_sdl3.h"
#include "imgui_impl_sdlrenderer3.h"
#include "sdl_window.hpp"
#include "web_view.hpp"

#include <SDL3/SDL.h>

#include <cstdio>
#include <cstdlib>

namespace quicksorter {

HomeWindow::HomeWindow(SdlWindowWrapper& sdl) : clear_color{ImVec4(0.45f, 0.55f, 0.60f, 1.00f)} {
    SDL_Texture* my_texture;
    int my_image_width, my_image_height;

    WebView wv {"https://www.example.com"};
    bool ret = quicksorter::LoadTextureFromFile(
        "/home/marcus/Devel/quicksorter/quicksorter-cpp/output.png",
        sdl.renderer(),
        &texture_,
        &image_width_,
        &image_height_);
    IM_ASSERT(ret);
}

HomeWindow::~HomeWindow() {
}

void HomeWindow::DisplayWindow() {
    static float f = 0.0f;
    static int counter = 0;

    ImGui::Begin("Hello, world!");                          // Create a window called "Hello, world!" and append into it.

    ImGui::Text("This is some useful text.");               // Display some text (you can use a format strings too)

    ImGui::SliderFloat("float", &f, 0.0f, 1.0f);            // Edit 1 float using a slider from 0.0f to 1.0f
    ImGui::ColorEdit3("clear color", (float*)&clear_color); // Edit 3 floats representing a color

    if (ImGui::Button("Button"))                            // Buttons return true when clicked (most widgets return true when edited/activated)
      counter++;
    ImGui::SameLine();
    ImGui::Text("counter = %d", counter);

    {
        ImGuiWindowFlags window_flags = ImGuiWindowFlags_HorizontalScrollbar;
        //if (disable_mouse_wheel)
        //    window_flags |= ImGuiWindowFlags_NoScrollWithMouse;
        ImGui::PushStyleVar(ImGuiStyleVar_ChildRounding, 5.0f);
        ImGui::BeginChild("ChildL", ImVec2(ImGui::GetContentRegionAvail().x * 0.5f, 260), ImGuiChildFlags_Borders, window_flags);
        //for (int i = 0; i < 100; i++)
        //    ImGui::Text("%04d: scrollable region", i);
        if (ImGui::BeginTable("table1", 3))
        {
            for (int row = 0; row < 50; row++)
            {
                ImGui::TableNextRow();
                for (int column = 0; column < 3; column++)
                {
                    ImGui::TableSetColumnIndex(column);
                    ImGui::Text("Row %d Column %d", row, column);
                }
            }
            ImGui::EndTable();
        }
        ImGui::EndChild();
        ImGui::PopStyleVar();
    }

    {
        ImGui::Begin("SDL_Renderer Texture Test");
        ImGui::Text("pointer = %p", texture_);
        ImGui::Text("size = %d x %d", image_width_, image_height_);
        ImGui::Image((ImTextureID)(intptr_t)texture_, ImVec2((float)image_width_, (float)image_height_));
        ImGui::End();
    }

    auto& io = ImGui::GetIO();
    ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / io.Framerate, io.Framerate);
    ImGui::End();
}

}  // namespace quicksorter
