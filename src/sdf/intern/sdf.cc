#include "sdf/sdf.hh"

#include <algorithm>

namespace sdf {

/* The shared GLSL formulas, compiled as C++. */
namespace glsl {
using namespace glm;
#include "sdf/glsl/sdf_library.glsl"
}  // namespace glsl

static float primitive_distance(const Primitive &prim, base::vec3 p)
{
  const base::vec3 local = base::vec3(prim.inverse_transform * base::vec4(p, 1.0f));
  switch (prim.type) {
    case PRIM_SPHERE:
      return glsl::sd_sphere(local, prim.params[0]);
    case PRIM_BOX:
      /* Params are the full size; the formula wants half of it. */
      return glsl::sd_box(local, base::vec3(prim.params[0], prim.params[1], prim.params[2]) * 0.5f);
    default:
      return 1e10f;
  }
}

float distance(const Program &program, base::vec3 p)
{
  float d = 1e10f;
  for (const Primitive &prim : program.prims) {
    /* TODO: the other combine operators. */
    d = std::min(d, primitive_distance(prim, p));
  }
  return d;
}

}  // namespace sdf
