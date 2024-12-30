#include "home_window.hpp"

#include "images.hpp"
#include "imgui.h"
#include "imgui_impl_sdl3.h"
#include "imgui_impl_sdlrenderer3.h"
#include "sdl_window.hpp"
#include "web_view.hpp"

#include <SDL3/SDL.h>

#include <algorithm>
#include <cassert>
#include <cstdio>
#include <cstdlib>
#include <cstring>

namespace quicksorter {

HomeWindow::HomeWindow(SdlWindowWrapper &sdl)
    : clear_color{ImVec4(0.45f, 0.55f, 0.60f, 1.00f)}, sdl_(sdl) {
  auto copy_size = std::max<size_t>(strlen(kDefaultUrl), kMaxUrlSize);
  std::memcpy(url_.data(), kDefaultUrl, copy_size);

  UpdateWebsite(url_.data());
}

HomeWindow::~HomeWindow() {}

void HomeWindow::DisplayWindow() {
  ImGui::Begin("QuickSorter"); // Create a window called "Hello, world!" and
                               // append into it.

  const char *items[] = {
      "AAAA",
      "BBBB",
      "CCCC",
      "DDDD",
      "EEEE",
      "FFFF",
      "GGGG",
      "HHHH",
      "IIII",
      "JJJJ",
      "KKKK",
      "LLLLLLL",
      "MMMM",
      "OOOOOOO",
      "https://www.xkcd.com",
      "https://www.google.com",
      "https://en.cppreference.com/w/cpp/container/array",
      "https://en.cppreference.com/w/cpp/container/vector",
      "https://news.ycombinator.com/item?id=42514637",
      "EEEE2",
      "FFFF2",
      "GGGG2",
      "HHHH2",
      "IIII2",
      "JJJJ2",
      "KKKK2",
      "EEEE3",
      "FFFF3",
      "GGGG3",
      "HHHH3",
      "IIII3",
      "JJJJ3",
      "KKKK3",
      "EEEE4",
      "FFFF4",
      "GGGG4",
      "HHHH4",
      "IIII4",
      "JJJJ4",
      "KKKK4",
      "EEEE5",
      "FFFF5",
      "GGGG5",
      "HHHH5",
      "IIII5",
      "JJJJ5",
      "KKKK5",
      "EEEE6",
      "FFFF6",
      "GGGG6",
      "HHHH6",
      "IIII6",
      "JJJJ6",
      "KKKK6",
      "EEEE7",
      "FFFF7",
      "GGGG7",
      "HHHH7",
      "IIII7",
      "JJJJ7",
      "KKKK7",
      "EEEE8",
      "FFFF8",
      "GGGG8",
      "HHHH8",
      "IIII8",
      "JJJJ8",
      "KKKK8",
  };
  static int item_selected_idx =
      0; // Here we store our selected data as an index.
  static bool item_highlight = false;
  int item_highlighted_idx =
      -1; // Here we store our highlighted data as an index.

  ImGui::Text("URL:");
  ImGui::SameLine();
  ImGui::InputText("##edit", url_.data(), url_.size());
  ImGui::SameLine();
  if (ImGui::Button("Update Website"))
    UpdateWebsite(url_.data());
  {
    ImGuiWindowFlags window_flags = ImGuiWindowFlags_HorizontalScrollbar;
    // if (disable_mouse_wheel)
    //     window_flags |= ImGuiWindowFlags_NoScrollWithMouse;
    ImGui::PushStyleVar(ImGuiStyleVar_ChildRounding, 5.0f);
    {
      auto height = ((ImGui::GetContentRegionAvail().y * 0.6f) /
                     (ImGui::GetTextLineHeightWithSpacing())) *
                    ImGui::GetTextLineHeightWithSpacing();
      //auto height = ImGui::GetContentRegionAvail().y * 0.6f;
      // Can use -FLT_MIN for max width?
      if (ImGui::BeginListBox(
              "#listbox",
              ImVec2(ImGui::GetContentRegionAvail().x * 0.5f, height))) {
        for (int n = 0; n < IM_ARRAYSIZE(items); n++) {
          bool is_selected = (item_selected_idx == n);
          ImGuiSelectableFlags flags =
              (item_highlighted_idx == n) ? ImGuiSelectableFlags_Highlight : 0;
          if (ImGui::Selectable(items[n], is_selected, flags)) {
            item_selected_idx = n;
            std::memset(url_.data(), 0, url_.size());
            const char *str = items[item_selected_idx];
            assert(strlen(str) < kMaxUrlSize);
            std::memcpy(url_.data(), items[item_selected_idx], strlen(str));
            UpdateWebsite(url_.data());
          }

          // Set the initial focus when opening the combo (scrolling + keyboard
          // navigation focus)
          if (is_selected)
            ImGui::SetItemDefaultFocus();
        }
        ImGui::EndListBox();
      }
    }
    ImGui::SameLine();
    {
      ImGui::BeginChild("#webview",
                        ImVec2(ImGui::GetContentRegionAvail().x,
                               ImGui::GetContentRegionAvail().y * 0.6f),
                        ImGuiChildFlags_Borders, window_flags);
      ImGui::Image((ImTextureID)(intptr_t)texture_,
                   ImVec2((float)image_width_, (float)image_height_));
      ImGui::EndChild();
    }
    ImGui::PopStyleVar();
  }
  {
    enum Mode { Mode_Copy, Mode_Move, Mode_Swap };
    static int mode = 0;
    if (ImGui::RadioButton("Copy", mode == Mode_Copy)) {
      mode = Mode_Copy;
    }
    ImGui::SameLine();
    if (ImGui::RadioButton("Move", mode == Mode_Move)) {
      mode = Mode_Move;
    }
    ImGui::SameLine();
    if (ImGui::RadioButton("Swap", mode == Mode_Swap)) {
      mode = Mode_Swap;
    }
    static const char *names[12] = {"priority/on-deck",
                                    "priority/shortlist",
                                    "priority/normal",
                                    "concern/tech",
                                    "concern/politics",
                                    "concern/history",
                                    "utility/send-to",
                                    "utility/inspo",
                                    "utility/reference",
                                    "Alice",
                                    "Bob",
                                    "Carol"};
    for (int n = 0; n < IM_ARRAYSIZE(names); n++) {
      ImGui::PushID(n);
      if ((n % 3) != 0)
        ImGui::SameLine();
      ImGui::Button(names[n], ImVec2(60, 60));

      // Our buttons are both drag sources and drag targets here!
      if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_None)) {
        // Set payload to carry the index of our item (could be anything)
        ImGui::SetDragDropPayload("DND_DEMO_CELL", &n, sizeof(int));

        // Display preview (could be anything, e.g. when dragging an image we
        // could decide to display the filename and a small preview of the
        // image, etc.)
        if (mode == Mode_Copy) {
          ImGui::Text("Copy %s", names[n]);
        }
        if (mode == Mode_Move) {
          ImGui::Text("Move %s", names[n]);
        }
        if (mode == Mode_Swap) {
          ImGui::Text("Swap %s", names[n]);
        }
        ImGui::EndDragDropSource();
      }
      if (ImGui::BeginDragDropTarget()) {
        if (const ImGuiPayload *payload =
                ImGui::AcceptDragDropPayload("DND_DEMO_CELL")) {
          IM_ASSERT(payload->DataSize == sizeof(int));
          int payload_n = *(const int *)payload->Data;
          if (mode == Mode_Copy) {
            names[n] = names[payload_n];
          }
          if (mode == Mode_Move) {
            names[n] = names[payload_n];
            names[payload_n] = "";
          }
          if (mode == Mode_Swap) {
            const char *tmp = names[n];
            names[n] = names[payload_n];
            names[payload_n] = tmp;
          }
        }
        ImGui::EndDragDropTarget();
      }
      ImGui::PopID();
    }
  }

  auto &io = ImGui::GetIO();
  ImGui::Text("Application average %.3f ms/frame (%.1f FPS)",
              1000.0f / io.Framerate, io.Framerate);
  ImGui::End();
}

void HomeWindow::UpdateWebsite(const char *website) {
  // TODO get display dimensions somehow
  // GdkDisplay *display = gdk_display_get_default();
  // GdkMonitor *monitor = gdk_display_get_primary_monitor(display);
  // GdkRectangle geometry;
  // gdk_monitor_get_geometry(monitor, &geometry);
  // gtk_window_set_default_size(GTK_WINDOW(window_), geometry.width,
  // geometry.height);
  WebView wv{website, 1200, 1600};
  bool ret = quicksorter::LoadTextureFromMemory(wv.data(), wv.data_size(),
                                                sdl_.renderer(), &texture_,
                                                &image_width_, &image_height_);
  IM_ASSERT(ret);
}

} // namespace quicksorter
