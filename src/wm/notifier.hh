#pragma once

/* "Something changed": tells editors which areas must redraw.
 * See ai/code-structure.md, "Notifiers and versions". */

#include "base/id.hh"

namespace wm {

enum class Category { All, Window, Screen, Scene, Object, Selection, Preferences };
enum class Action { Changed, Added, Removed };

struct Notifier {
  Category category = Category::All;
  Action action = Action::Changed;
  base::Id id = base::NO_ID; /* e.g. the object that changed */
};

}  // namespace wm
