#pragma once

/* A compiled GLSL program.
 * TODO(roadmap 5): compile, link, set uniforms. The ray march engine is the first user. */

namespace gpu {

class Shader {
 private:
  unsigned program_ = 0;
};

}  // namespace gpu
