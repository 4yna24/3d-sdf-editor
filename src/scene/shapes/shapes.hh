#pragma once

/* One register function per file in this folder. Add a new shape here and in register_shapes(). */

#include "scene/shape_type.hh"

namespace scene {

void register_sphere(ShapeTypes &types);
void register_box(ShapeTypes &types);

}  // namespace scene
