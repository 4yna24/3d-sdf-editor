/* Startup and shutdown. See ai/code-structure.md, "Startup". */

#include "app/app.hh"
#include "gpu/gpu.hh"
#include "platform/platform.hh"
#include "prefs/prefs.hh"
#include "scene/document.hh"
#include "ui/ui.hh"
#include "wm/window_manager.hh"

int main()
{
  if (!platform::init()) {
    return 1;
  }
  app::register_all();

  prefs::Preferences prefs;
  prefs::load(prefs);
  scene::Document doc = scene::new_document();

  ui::init();
  int exit_code = 0;
  {
    wm::WindowManager wm(doc, prefs);
    const int layout = wm.add_workspace(app::default_layout());

    if (wm.open_window("SDF Editor", layout) && gpu::init(platform::gl_loader())) {
      ui::init_renderer();
      wm.run();
      ui::shutdown_renderer();
    }
    else {
      exit_code = 1;
    }
    wm.close_windows();
  }
  ui::shutdown();
  platform::shutdown();
  return exit_code;
}
