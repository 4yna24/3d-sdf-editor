#pragma once

/* Shape types: one small registered entry per SDF shape.
 * The formula is in sdf/glsl/sdf_library.glsl, the description is in scene/shapes/<shape>.cc. */

#include <deque>
#include <string>

#include "props/props.hh"
#include "scene/object.hh"
#include "sdf/sdf.hh"

namespace scene {

struct ShapeType {
  ShapeTypeId id;    /* "sphere" */
  std::string label; /* "Sphere" */
  std::string icon;
  sdf::PrimType prim = sdf::PRIM_SPHERE;
  props::StructDesc params; /* The shape's numbers, in the order they sit in SdfShape::params. */
};

class ShapeTypes {
 public:
  ShapeType &add(ShapeTypeId id, std::string label, sdf::PrimType prim);
  const ShapeType *find(const ShapeTypeId &id) const;
  const std::deque<ShapeType> &all() const
  {
    return types_;
  }

 private:
  std::deque<ShapeType> types_; /* deque: references stay valid when adding. */
};

/** The global registry, filled once at startup. */
ShapeTypes &shape_types();

/** Register every built-in shape. Called by app at startup. */
void register_shapes();

/** A new shape of a registered type, with its default values. */
SdfShape new_shape(const ShapeTypeId &type);

}  // namespace scene
