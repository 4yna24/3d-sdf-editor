# The only module that uses SDL. Dear ImGui's SDL backend lives here too.
sdfe_add_module(platform
  SOURCES
    intern/platform.cc
    intern/event.cc
  USES
    base
  LIBS
    SDL3::SDL3
    imgui_backend_sdl3
)
