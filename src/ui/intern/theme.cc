#include "ui/theme.hh"

#include <imgui.h>

namespace ui::theme {

void apply()
{
  ImGui::StyleColorsDark();
  ImGuiStyle &s = ImGui::GetStyle();
  s.WindowRounding = 0.0f;
  s.WindowBorderSize = 0.0f;
  s.WindowMinSize = {1.0f, 1.0f}; /* Headers are one line high; the default (32) would overlap. */
  s.FrameRounding = 3.0f;
  s.PopupRounding = 3.0f;
  s.PopupBorderSize = 1.0f;

  /* Grays close to Blender's default dark theme, blue for "selected / on". */
  const ImVec4 blue{0.28f, 0.45f, 0.70f, 1.0f};
  ImVec4 *c = s.Colors;
  c[ImGuiCol_WindowBg] = {0.19f, 0.19f, 0.19f, 1.0f};
  c[ImGuiCol_MenuBarBg] = {0.15f, 0.15f, 0.15f, 1.0f};
  c[ImGuiCol_PopupBg] = {0.11f, 0.11f, 0.11f, 0.98f};
  c[ImGuiCol_Button] = {0.28f, 0.28f, 0.28f, 1.0f};
  c[ImGuiCol_ButtonHovered] = {0.36f, 0.36f, 0.36f, 1.0f};
  c[ImGuiCol_ButtonActive] = blue;
  c[ImGuiCol_Header] = blue;
  c[ImGuiCol_HeaderHovered] = blue;
  c[ImGuiCol_HeaderActive] = blue;
}

base::vec4 background()
{
  return {0.08f, 0.08f, 0.08f, 1.0f};
}

}  // namespace ui::theme
