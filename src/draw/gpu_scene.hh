#pragma once

/* The scene on the GPU, shared by all viewports.
 * TODO(roadmap 5): keep the SDF program in a gpu::Buffer; update only objects whose version changed. */

#include "scene/scene.hh"

namespace draw {

class GpuScene {
 public:
  void sync(const scene::Scene &scene);

 private:
  uint64_t synced_version_ = 0;
};

}  // namespace draw
