#pragma once

/* What every editor and operator gets. wm builds one for each event and each draw. */

#include "prefs/prefs.hh"
#include "scene/document.hh"
#include "wm/notifier.hh"

namespace wm {

class WindowManager;
struct Window;
struct Area;
struct Screen;

struct Context {
  WindowManager &wm;
  Window *window = nullptr; /* Where it happens. May be null. */
  Area *area = nullptr;     /* May be null (e.g. a window hotkey with the mouse on no area). */
  scene::Document &doc;
  prefs::Preferences &prefs;

  scene::Scene &scene()
  {
    return doc.scene;
  }
  scene::Object *active_object();
  Screen *screen(); /* The area layout of `window`. */
  void notify(const Notifier &n);
};

}  // namespace wm
