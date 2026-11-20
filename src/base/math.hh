#pragma once

/* Math types for the whole program.
 * GLM uses the same names as GLSL, so SDF code can be shared by the CPU and the GPU. */

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>

namespace base {

using glm::mat4;
using glm::quat;
using glm::vec2;
using glm::vec3;
using glm::vec4;

}  // namespace base
