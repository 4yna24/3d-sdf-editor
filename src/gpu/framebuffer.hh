#pragma once

/* Something to draw into: a color texture + depth. Each 3D viewport draws into its own.
 * TODO(roadmap 5). */

namespace gpu {

class Framebuffer {
 private:
  unsigned id_ = 0;
};

}  // namespace gpu
