#pragma once

/* Every change and every action from a menu or hotkey is an operator.
 * See ai/code-structure.md, "Operators". */

#include <functional>
#include <string>

#include "platform/event.hh"
#include "props/props.hh"
#include "wm/context.hh"

namespace wm {

using platform::Event;

enum class Result { Finished, Cancelled, Running, PassThrough };

struct OperatorType {
  std::string id;          /* "object.delete" */
  std::string label;       /* "Delete": menus, undo history, search. */
  std::string description; /* Tooltip. */
  props::StructDesc args;  /* Its inputs. */
  bool undo = false;       /* Push one undo step when it finishes. */

  std::function<bool(const Context &)> poll;                                /* Can it run? */
  std::function<Result(Context &, props::Values &)> exec;                   /* The work. */
  std::function<Result(Context &, props::Values &, const Event &)> invoke;  /* From mouse/key. */
  std::function<Result(Context &, props::Values &, const Event &)> modal;   /* While Running. */
};

void register_operator(OperatorType type);
const OperatorType *find_operator(const std::string &id);

}  // namespace wm
