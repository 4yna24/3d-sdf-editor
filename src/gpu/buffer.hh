#pragma once

/* A buffer on the GPU. The SDF program is uploaded into one.
 * TODO(roadmap 5): create, upload, update one range. */

namespace gpu {

class Buffer {
 private:
  unsigned id_ = 0;
};

}  // namespace gpu
