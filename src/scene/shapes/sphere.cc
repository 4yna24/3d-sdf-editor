#include "scene/shapes/shapes.hh"

namespace scene {

void register_sphere(ShapeTypes &types)
{
  ShapeType &t = types.add("sphere", "Sphere", sdf::PRIM_SPHERE);
  t.icon = "SPHERE";
  t.params.add_float("radius", "Radius", 1.0f).range(0.001f, 1000.0f).set_unit(props::Unit::Length);
}

}  // namespace scene
