#include "scene/shapes/shapes.hh"

namespace scene {

void register_box(ShapeTypes &types)
{
  ShapeType &t = types.add("box", "Box", sdf::PRIM_BOX);
  t.icon = "CUBE";
  t.params.add_float("size_x", "Size X", 2.0f).range(0.001f, 1000.0f).set_unit(props::Unit::Length);
  t.params.add_float("size_y", "Size Y", 2.0f).range(0.001f, 1000.0f).set_unit(props::Unit::Length);
  t.params.add_float("size_z", "Size Z", 2.0f).range(0.001f, 1000.0f).set_unit(props::Unit::Length);
}

}  // namespace scene
