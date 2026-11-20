#pragma once

#include <map>
#include <set>
#include <string>
#include <vector>

#include "scene/object.hh"

namespace scene {

/** For organising, as in Blender. Does not change any shape. */
struct Collection {
  std::string name;
  std::vector<ObjectId> objects;
  std::vector<Collection> children;
};

struct Selection {
  std::set<ObjectId> selected;
  ObjectId active = base::NO_ID; /* The one selected last. The Properties editor shows it. */
};

struct World {
  base::vec3 background{0.05f};
  float ambient = 0.1f;
};

struct Scene {
  std::map<ObjectId, Object> objects;
  Collection root{"Scene Collection", {}, {}};
  Selection selection;
  World world;
  ObjectId next_id = 1;
  uint64_t version = 0;
};

/* Change the scene only through these functions: they keep ids, collections and versions right. */

Object &add_object(Scene &scene, std::string name, Object::Data data);
bool remove_object(Scene &scene, ObjectId id);
Object *find_object(Scene &scene, ObjectId id);
const Object *find_object(const Scene &scene, ObjectId id);

/* TODO(roadmap 3): select, rename, set_transform, collections. */

}  // namespace scene
