#include "wm/context.hh"

#include "wm/window_manager.hh"

namespace wm {

scene::Object *Context::active_object()
{
  return scene::find_object(doc.scene, doc.scene.selection.active);
}

Screen *Context::screen()
{
  return window ? wm.screen(*window) : nullptr;
}

void Context::notify(const Notifier &n)
{
  wm.notify(n);
}

}  // namespace wm
