#pragma once

/* Drawn on top of every engine: ground grid, axes, light and camera icons, selection outline.
 * TODO(roadmap 5). */

#include "draw/view.hh"
#include "scene/scene.hh"

namespace draw {

void draw_overlays(const scene::Scene &scene, const View &view);

}  // namespace draw
