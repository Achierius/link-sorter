#pragma once

#include "imgui.h"
#include "imgui_impl_sdl3.h"
#include "imgui_impl_sdlrenderer3.h"
#include "sdl_window.hpp"

#include <SDL3/SDL.h>

namespace quicksorter {

class HomeWindow {
 public:
  HomeWindow(SdlWindowWrapper& sdl);
  ~HomeWindow();

  void DisplayWindow();
  void UpdateWebsite(const char* website);

  ImVec4 clear_color;
 private:

  SdlWindowWrapper& sdl_;
  SDL_Texture* texture_;
  int image_width_;
  int image_height_;
};

}  // namespace quicksorter
