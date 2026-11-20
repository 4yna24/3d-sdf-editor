# Widgets on top of Dear ImGui. The only module that includes ImGui (besides its SDL backend in
# platform). Editors draw with ui:: functions, never with ImGui directly.
sdfe_add_module(ui
  SOURCES
    intern/ui.cc
    intern/widgets.cc
    intern/theme.cc
  USES
    props
    gpu
    base
  LIBS
    imgui
    imgui_backend_opengl3
)
