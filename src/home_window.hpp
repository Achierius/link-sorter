#pragma once

#include <array>

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
  static constexpr const char* kDefaultUrl { "https://en.cppreference.com/w/cpp/container/vector" };
  static inline constexpr size_t kMaxUrlSize {64};  // TODO purposefully small so I
                                             // handle this sooner not later
  std::array<char, kMaxUrlSize> url_ {};

  SdlWindowWrapper& sdl_;
  SDL_Texture* texture_;

  int image_width_ {0};
  int image_height_ {0};
};

}  // namespace quicksorter
