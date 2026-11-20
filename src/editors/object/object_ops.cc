#include "editors/object/object_ops.hh"

#include "wm/operator.hh"

namespace editors::object {

void register_operators()
{
  /* TODO(roadmap 3/4):
   *   object.add       args: shape type. Uses scene::add_object(). undo = true.
   *   object.delete    poll: something is selected. undo = true.
   *   object.duplicate
   *   object.rename
   *   object.select    (Outliner click; the viewport uses view3d.select) */
}

}  // namespace editors::object
