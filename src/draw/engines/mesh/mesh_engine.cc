#include "draw/engines/mesh/mesh_engine.hh"

#include "draw/engine.hh"

namespace draw {

/* TODO(roadmap 7): one mesh per object from sdf/mesher.hh, rebuilt when the object's version changes. */
class MeshEngine : public Engine {
 public:
  void sync(const scene::Scene & /*scene*/) override {}
  void draw(const View & /*view*/, gpu::Framebuffer & /*target*/) override {}
};

void register_mesh_engine()
{
  register_engine({"MESH", "Mesh", [] { return std::make_unique<MeshEngine>(); }});
}

}  // namespace draw
