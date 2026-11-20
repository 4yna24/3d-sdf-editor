#include "app/app.hh"

#include "draw/engine.hh"
#include "editors/file/file_ops.hh"
#include "editors/object/object_ops.hh"
#include "editors/screen/screen_ops.hh"
#include "editors/space_outliner/space_outliner.hh"
#include "editors/space_preferences/space_preferences.hh"
#include "editors/space_properties/space_properties.hh"
#include "editors/space_topbar/space_topbar.hh"
#include "editors/space_view3d/space_view3d.hh"
#include "scene/shape_type.hh"
#include "wm/wm_operators.hh"

namespace app {

/* Adding a module, editor or engine? Add one line here. */
void register_all()
{
  scene::register_shapes();
  draw::register_engines();

  wm::register_types();
  editors::screen::register_operators();
  editors::file::register_operators();
  editors::object::register_operators();

  /* The order here is the order in the editor type button. */
  editors::topbar::register_type();
  editors::view3d::register_type();
  editors::outliner::register_type();
  editors::properties::register_type();
  editors::preferences::register_type();
}

}  // namespace app
