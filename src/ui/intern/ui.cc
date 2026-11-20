#include "ui/ui.hh"

#include <imgui.h>
#include <imgui_impl_opengl3.h>

#include "gpu/gpu.hh"
#include "ui/theme.hh"

namespace ui {

void init()
{
  IMGUI_CHECKVERSION();
  ImGui::CreateContext();
  ImGuiIO &io = ImGui::GetIO();
  io.IniFilename = nullptr; /* The layout is ours (wm). ImGui saves nothing. */
  theme::apply();
}

void init_renderer()
{
  ImGui_ImplOpenGL3_Init(gpu::glsl_version());
}

void shutdown_renderer()
{
  ImGui_ImplOpenGL3_Shutdown();
}

void shutdown()
{
  ImGui::DestroyContext();
}

void begin_frame()
{
  ImGui_ImplOpenGL3_NewFrame();
  ImGui::NewFrame();
}

void end_frame()
{
  ImGui::Render();
  const ImGuiIO &io = ImGui::GetIO();
  const int w = int(io.DisplaySize.x * io.DisplayFramebufferScale.x);
  const int h = int(io.DisplaySize.y * io.DisplayFramebufferScale.y);
  gpu::set_viewport(0, 0, w, h);
  gpu::clear(theme::background());
  ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

float header_height()
{
  return ImGui::GetFrameHeight();
}

bool wants_mouse()
{
  return ImGui::IsAnyItemHovered() || ImGui::IsAnyItemActive();
}

bool wants_keyboard()
{
  return ImGui::GetIO().WantTextInput;
}

bool popup_open()
{
  return ImGui::IsPopupOpen("", ImGuiPopupFlags_AnyPopupId | ImGuiPopupFlags_AnyPopupLevel);
}

void set_cursor(Cursor cursor)
{
  switch (cursor) {
    case Cursor::Arrow: ImGui::SetMouseCursor(ImGuiMouseCursor_Arrow); break;
    case Cursor::ResizeEW: ImGui::SetMouseCursor(ImGuiMouseCursor_ResizeEW); break;
    case Cursor::ResizeNS: ImGui::SetMouseCursor(ImGuiMouseCursor_ResizeNS); break;
  }
}

}  // namespace ui
