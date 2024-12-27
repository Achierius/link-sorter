#pragma once

#include <cstdio>
#include <cstdlib>

#include "imgui.h"
#include "imgui_impl_sdl3.h"
#include "imgui_impl_sdlrenderer3.h"

#include <SDL3/SDL.h>

namespace quicksorter {

class SdlWindowWrapper {
 public:
  SdlWindowWrapper() {
      // Setup SDL
      // TODO move this out or wrap it in a mutex
      if (!SDL_Init(SDL_INIT_VIDEO | SDL_INIT_GAMEPAD))
      {
          printf("Error: SDL_Init(): %s\n", SDL_GetError());
          std::abort();
      }

      // Create window with SDL_Renderer graphics context
      window_ = SDL_CreateWindow("Dear ImGui SDL3+SDL_Renderer example", kWindowWidth, kWindowHeight, kWindowFlags);
      if (window_ == nullptr) {
          printf("Error: SDL_CreateWindow(): %s\n", SDL_GetError());
          std::abort();
      }
      renderer_ = SDL_CreateRenderer(window_, nullptr);
      SDL_SetRenderVSync(renderer_, 1);
      if (renderer_ == nullptr) {
          SDL_Log("Error: SDL_CreateRenderer(): %s\n", SDL_GetError());
          std::abort();
      }
      SDL_SetWindowPosition(window_, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED);
      SDL_ShowWindow(window_);
  }

  ~SdlWindowWrapper() {
      SDL_DestroyRenderer(renderer_);
      SDL_DestroyWindow(window_);
      SDL_Quit();
  }

  SDL_Renderer* renderer() { return renderer_; }
  SDL_Window* window() { return window_; }

 private:
  static constexpr uint32_t kWindowFlags = SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE | SDL_WINDOW_HIDDEN;
  static constexpr uint32_t kWindowWidth = 1280;
  static constexpr uint32_t kWindowHeight = 720;

  SDL_Renderer* renderer_;
  SDL_Window* window_;
};

}  // namespace quicksorter
