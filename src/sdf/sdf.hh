#pragma once

/* The SDF program: a flat list of primitives that the CPU and the GPU both evaluate.
 * See ai/rendering.md, "The SDF program". */

#include <cstdint>
#include <vector>

#include "base/math.hh"

namespace sdf {

/** Primitive types. The numbers must match the formulas in glsl/sdf_library.glsl. */
enum PrimType : int {
  PRIM_SPHERE = 0,
  PRIM_BOX = 1,
};

/** How a primitive joins the shapes before it. Only Union for now. */
enum class Combine : int {
  Union = 0,
  /* Later: SmoothUnion, Subtract, Intersect (with `blend`). */
};

/** One entry of the program. Kept plain so it can be uploaded to the GPU as is. */
struct Primitive {
  int type = PRIM_SPHERE;
  Combine combine = Combine::Union;
  float blend = 0.0f;
  uint32_t object_id = 0;
  float params[8] = {};
  base::mat4 inverse_transform{1.0f};
  base::vec4 color{0.8f, 0.8f, 0.8f, 1.0f};
};

struct Program {
  std::vector<Primitive> prims;
};

/** Distance from `p` (world space) to the nearest surface of the program. */
float distance(const Program &program, base::vec3 p);

}  // namespace sdf
