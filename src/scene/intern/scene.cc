#include "scene/scene.hh"

#include <algorithm>

namespace scene {

Object &add_object(Scene &scene, std::string name, Object::Data data)
{
  const ObjectId id = scene.next_id++;
  Object &ob = scene.objects[id];
  ob.id = id;
  ob.name = std::move(name);
  ob.data = std::move(data);
  scene.root.objects.push_back(id);
  scene.version++;
  return ob;
}

static void remove_from_collection(Collection &c, ObjectId id)
{
  std::erase(c.objects, id);
  for (Collection &child : c.children) {
    remove_from_collection(child, id);
  }
}

bool remove_object(Scene &scene, ObjectId id)
{
  if (scene.objects.erase(id) == 0) {
    return false;
  }
  remove_from_collection(scene.root, id);
  scene.selection.selected.erase(id);
  if (scene.selection.active == id) {
    scene.selection.active = base::NO_ID;
  }
  scene.version++;
  return true;
}

Object *find_object(Scene &scene, ObjectId id)
{
  auto it = scene.objects.find(id);
  return it == scene.objects.end() ? nullptr : &it->second;
}

const Object *find_object(const Scene &scene, ObjectId id)
{
  auto it = scene.objects.find(id);
  return it == scene.objects.end() ? nullptr : &it->second;
}

}  // namespace scene
