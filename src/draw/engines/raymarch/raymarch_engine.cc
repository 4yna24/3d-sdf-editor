#include "draw/engines/raymarch/raymarch_engine.hh"

#include "draw/engine.hh"

namespace draw {

/* TODO(roadmap 5): full screen triangle + shaders/raymarch.frag.glsl, reading the GpuScene buffer. */
class RaymarchEngine : public Engine {
 public:
  void sync(const scene::Scene & /*scene*/) override {}
  void draw(const View & /*view*/, gpu::Framebuffer & /*target*/) override {}
};

void register_raymarch_engine()
{
  register_engine({"RAYMARCH", "Ray march", [] { return std::make_unique<RaymarchEngine>(); }});
}

}  // namespace draw
