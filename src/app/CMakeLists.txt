# main(). The only place that knows every module.
add_executable(sdf_editor
  main.cc
  register.cc
  default_layout.cc
)
target_link_libraries(sdf_editor PRIVATE
  sdfe_editors sdfe_wm sdfe_draw sdfe_io sdfe_ui sdfe_scene sdfe_prefs
  sdfe_sdf sdfe_props sdfe_gpu sdfe_platform sdfe_base
)
sdfe_set_warnings(sdf_editor)
