#include "draw/gpu_scene.hh"

namespace draw {

void GpuScene::sync(const scene::Scene &scene)
{
  if (scene.version == synced_version_) {
    return;
  }
  /* TODO(roadmap 5): upload scene::sdf_program(scene). */
  synced_version_ = scene.version;
}

}  // namespace draw
