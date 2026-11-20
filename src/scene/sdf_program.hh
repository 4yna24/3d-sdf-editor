#pragma once

#include "scene/scene.hh"
#include "sdf/sdf.hh"

namespace scene {

/** The visible SDF objects as a flat list, for the ray marcher, the mesher and picking.
 * TODO(roadmap 5): cache it and rebuild only objects whose version changed. */
sdf::Program sdf_program(const Scene &scene);

}  // namespace scene
