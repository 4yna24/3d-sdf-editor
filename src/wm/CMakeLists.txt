# Window manager: windows, area layout, main loop, events, operators, keymaps, menus, undo.
sdfe_add_module(wm
  SOURCES
    intern/window_manager.cc
    intern/screen.cc
    intern/editor.cc
    intern/operator.cc
    intern/keymap.cc
    intern/menu.cc
    intern/context.cc
    intern/undo.cc
    intern/wm_operators.cc
  USES
    ui
    platform
    prefs
    scene
    props
    base
)

sdfe_add_tests(wm SOURCES tests/test_screen.cc)
