#include "wm/undo.hh"

namespace wm {

/* TODO(roadmap 3): keep a list of scene copies and a position in it. */

void UndoStack::push(const scene::Scene & /*scene*/, const std::string & /*label*/) {}

bool UndoStack::can_undo() const
{
  return false;
}

bool UndoStack::can_redo() const
{
  return false;
}

bool UndoStack::undo(scene::Scene & /*scene*/)
{
  return false;
}

bool UndoStack::redo(scene::Scene & /*scene*/)
{
  return false;
}

}  // namespace wm
