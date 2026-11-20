#pragma once

/* What every editor type implements. wm draws every area the same way and calls these.
 * See ai/code-structure.md, "Windows, areas, editors". */

#include <functional>
#include <memory>
#include <string>
#include <vector>

#include "base/json.hh"
#include "base/rect.hh"
#include "platform/event.hh"
#include "wm/context.hh"

namespace wm {

using base::Rect;
using platform::Event;

class Editor {
 public:
  virtual ~Editor() = default;
  /** The editor's part of the area header, right of the editor type button. */
  virtual void draw_header(Context &) {}
  /** The main region of the area. */
  virtual void draw_main(Context &, Rect) = 0;
  /** True = "redraw me". */
  virtual bool on_notify(const Notifier &)
  {
    return true;
  }
  /** Rarely needed: most input goes through keymaps. True = used the event. */
  virtual bool handle_event(Context &, const Event &)
  {
    return false;
  }
  virtual base::json save_state() const
  {
    return {};
  }
  virtual void load_state(const base::json &) {}
};

struct EditorType {
  std::string id;     /* "VIEW_3D" */
  std::string label;  /* "3D Viewport": shown in the editor type button. */
  std::string icon;   /* Short text until there are icons. */
  std::string keymap; /* "View3D": used while the mouse is over this editor. */
  std::function<std::unique_ptr<Editor>()> create;
  bool in_type_menu = true; /* False for the top bar: it cannot be picked for an area. */
};

void register_editor_type(EditorType type);
const EditorType *find_editor_type(const std::string &id);
const std::vector<EditorType> &editor_types();

}  // namespace wm
