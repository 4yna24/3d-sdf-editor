#pragma once

/* An object: a shape, a light or a camera, with a name and a transform.
 * Plain data that can be copied (undo and background jobs rely on that). See ai/scene.md. */

#include <array>
#include <cstdint>
#include <string>
#include <variant>

#include "base/id.hh"
#include "base/math.hh"
#include "sdf/sdf.hh"

namespace scene {

using ObjectId = base::Id;
using ShapeTypeId = std::string; /* "sphere", "box": the id of a registered ShapeType. */

struct Transform {
  base::vec3 location{0.0f};
  base::quat rotation{1.0f, 0.0f, 0.0f, 0.0f};
  float scale = 1.0f; /* One number: uneven scale breaks SDF distances. */
};

struct Material {
  base::vec3 color{0.8f};
};

struct SdfShape {
  ShapeTypeId type;
  std::array<float, 8> params{}; /* What each number means is given by the shape type. */
  Material material;
  sdf::Combine combine = sdf::Combine::Union;
};

struct Light {
  enum class Type { Point, Sun };
  Type type = Type::Point;
  base::vec3 color{1.0f};
  float strength = 1.0f;
};

struct Camera {
  float fov_degrees = 50.0f;
};

struct Object {
  using Data = std::variant<SdfShape, Light, Camera>;

  ObjectId id = base::NO_ID;
  std::string name;
  bool visible = true;
  Transform transform;
  uint64_t version = 0; /* Goes up on every change. See ai/scene.md, "Change tracking". */
  Data data;
};

}  // namespace scene
