#include "scene/document.hh"

#include "scene/shape_type.hh"

namespace scene {

Document new_document()
{
  Document doc;
  Scene &scene = doc.scene;

  Object &sphere = add_object(scene, "Sphere", new_shape("sphere"));
  sphere.transform.location = {0.0f, 0.0f, 1.0f};

  Object &light = add_object(scene, "Light", Light{});
  light.transform.location = {4.0f, -4.0f, 6.0f};

  scene.selection.selected.insert(sphere.id);
  scene.selection.active = sphere.id;
  return doc;
}

}  // namespace scene
