#pragma once

/* Undo stores copies of the scene, one per finished operator with undo = true.
 * See ai/code-structure.md, "Undo". */

#include <string>
#include <vector>

#include "scene/scene.hh"

namespace wm {

class UndoStack {
 public:
  /** TODO(roadmap 3): store copies. Until then nothing can be undone. */
  void push(const scene::Scene &scene, const std::string &label);
  bool can_undo() const;
  bool can_redo() const;
  bool undo(scene::Scene &scene);
  bool redo(scene::Scene &scene);
};

}  // namespace wm
