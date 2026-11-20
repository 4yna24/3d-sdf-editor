#pragma once

#include <string>

#include "scene/scene.hh"

namespace scene {

/** What one file holds. */
struct Document {
  Scene scene;
  std::string filepath;
  bool modified = false;
};

/** The default scene: a sphere and a light. Shape types must be registered first. */
Document new_document();

}  // namespace scene
