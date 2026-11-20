#include "scene/sdf_program.hh"

#include "scene/shape_type.hh"

namespace scene {

static base::mat4 object_matrix(const Transform &t)
{
  return glm::translate(base::mat4(1.0f), t.location) * glm::mat4_cast(t.rotation) *
         glm::scale(base::mat4(1.0f), base::vec3(t.scale));
}

sdf::Program sdf_program(const Scene &scene)
{
  sdf::Program program;
  for (const auto &[id, ob] : scene.objects) {
    const SdfShape *shape = std::get_if<SdfShape>(&ob.data);
    if (!ob.visible || !shape) {
      continue;
    }
    const ShapeType *type = shape_types().find(shape->type);
    if (!type) {
      continue;
    }
    sdf::Primitive &prim = program.prims.emplace_back();
    prim.type = type->prim;
    prim.combine = shape->combine;
    prim.object_id = id;
    std::copy(shape->params.begin(), shape->params.end(), prim.params);
    /* TODO: scale also scales distances; multiply the result by scale. */
    prim.inverse_transform = glm::inverse(object_matrix(ob.transform));
    prim.color = base::vec4(shape->material.color, 1.0f);
  }
  return program;
}

}  // namespace scene
