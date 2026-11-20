#include "scene/shape_type.hh"

#include "base/log.hh"
#include "scene/shapes/shapes.hh"

namespace scene {

ShapeType &ShapeTypes::add(ShapeTypeId id, std::string label, sdf::PrimType prim)
{
  ShapeType &t = types_.emplace_back();
  t.id = std::move(id);
  t.label = std::move(label);
  t.prim = prim;
  return t;
}

const ShapeType *ShapeTypes::find(const ShapeTypeId &id) const
{
  for (const ShapeType &t : types_) {
    if (t.id == id) {
      return &t;
    }
  }
  return nullptr;
}

ShapeTypes &shape_types()
{
  static ShapeTypes types;
  return types;
}

void register_shapes()
{
  ShapeTypes &types = shape_types();
  if (!types.all().empty()) {
    return; /* Already done. */
  }
  register_sphere(types);
  register_box(types);
}

SdfShape new_shape(const ShapeTypeId &type)
{
  SdfShape shape;
  shape.type = type;
  const ShapeType *t = shape_types().find(type);
  if (!t) {
    base::log_warning("Unknown shape type '{}'", type);
    return shape;
  }
  for (size_t i = 0; i < t->params.props.size() && i < shape.params.size(); i++) {
    if (const float *v = std::get_if<float>(&t->params.props[i].default_value)) {
      shape.params[i] = *v;
    }
  }
  return shape;
}

}  // namespace scene
