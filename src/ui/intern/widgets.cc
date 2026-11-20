#include "ui/widgets.hh"

#include <algorithm>

#include <imgui.h>

namespace ui {

/* Regions are ImGui windows placed exactly on our rectangles. The user cannot move them. */
static constexpr ImGuiWindowFlags REGION_FLAGS =
    ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoSavedSettings |
    ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoFocusOnAppearing;

static bool g_menu_bar_open = false;

static void place_next_window(const base::Rect &r)
{
  ImGui::SetNextWindowPos({r.x, r.y});
  ImGui::SetNextWindowSize({r.w, r.h});
}

void begin_header(const char *id, const base::Rect &rect)
{
  place_next_window(rect);
  ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, {4.0f, 0.0f});
  ImGui::Begin(id, nullptr, REGION_FLAGS | ImGuiWindowFlags_MenuBar);
  ImGui::PopStyleVar();
  g_menu_bar_open = ImGui::BeginMenuBar();
}

void end_header()
{
  if (g_menu_bar_open) {
    ImGui::EndMenuBar();
    g_menu_bar_open = false;
  }
  ImGui::End();
}

void begin_region(const char *id, const base::Rect &rect)
{
  place_next_window(rect);
  ImGui::Begin(id, nullptr, REGION_FLAGS);
}

void end_region()
{
  ImGui::End();
}

void label(const std::string &text)
{
  ImGui::TextUnformatted(text.c_str());
}

void label_dim(const std::string &text)
{
  ImGui::TextDisabled("%s", text.c_str());
}

void centered_label(const std::string &text)
{
  const ImVec2 size = ImGui::GetWindowSize();
  const ImVec2 text_size = ImGui::CalcTextSize(text.c_str());
  ImGui::SetCursorPos({std::max(0.0f, (size.x - text_size.x) * 0.5f),
                       std::max(0.0f, (size.y - text_size.y) * 0.5f)});
  ImGui::TextDisabled("%s", text.c_str());
}

void same_line()
{
  ImGui::SameLine();
}

void align_right(float width)
{
  const float x = ImGui::GetWindowWidth() - width - ImGui::GetStyle().WindowPadding.x;
  if (x > ImGui::GetCursorPosX()) {
    ImGui::SetCursorPosX(x);
  }
}

float button_width(const std::string &label)
{
  const ImGuiStyle &style = ImGui::GetStyle();
  return ImGui::CalcTextSize(label.c_str(), nullptr, true).x + style.FramePadding.x * 2.0f +
         style.ItemSpacing.x;
}

bool button(const std::string &label)
{
  return ImGui::Button(label.c_str());
}

bool toggle(const std::string &label, bool active)
{
  if (active) {
    ImGui::PushStyleColor(ImGuiCol_Button, ImGui::GetStyleColorVec4(ImGuiCol_ButtonActive));
  }
  const bool clicked = ImGui::Button(label.c_str());
  if (active) {
    ImGui::PopStyleColor();
  }
  return clicked;
}

bool begin_menu(const std::string &label, bool enabled)
{
  return ImGui::BeginMenu(label.c_str(), enabled);
}

bool menu_item(const std::string &label, const std::string &shortcut, bool enabled, bool checked)
{
  return ImGui::MenuItem(
      label.c_str(), shortcut.empty() ? nullptr : shortcut.c_str(), checked, enabled);
}

void menu_separator()
{
  ImGui::Separator();
}

void end_menu()
{
  ImGui::EndMenu();
}

}  // namespace ui
