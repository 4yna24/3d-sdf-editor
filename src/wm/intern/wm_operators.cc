#include "wm/wm_operators.hh"

#include "wm/keymap.hh"
#include "wm/operator.hh"
#include "wm/window_manager.hh"

namespace wm {

static void register_quit()
{
  OperatorType ot;
  ot.id = "wm.quit";
  ot.label = "Quit";
  ot.description = "Quit the program";
  ot.exec = [](Context &ctx, props::Values &) {
    /* TODO: ask first when the document has unsaved changes. */
    ctx.wm.quit();
    return Result::Finished;
  };
  register_operator(std::move(ot));
}

static void register_undo()
{
  OperatorType ot;
  ot.id = "edit.undo";
  ot.label = "Undo";
  ot.poll = [](const Context &ctx) { return ctx.wm.undo_stack().can_undo(); };
  ot.exec = [](Context &ctx, props::Values &) {
    if (!ctx.wm.undo_stack().undo(ctx.scene())) {
      return Result::Cancelled;
    }
    ctx.notify({Category::All});
    return Result::Finished;
  };
  register_operator(std::move(ot));
}

static void register_redo()
{
  OperatorType ot;
  ot.id = "edit.redo";
  ot.label = "Redo";
  ot.poll = [](const Context &ctx) { return ctx.wm.undo_stack().can_redo(); };
  ot.exec = [](Context &ctx, props::Values &) {
    if (!ctx.wm.undo_stack().redo(ctx.scene())) {
      return Result::Cancelled;
    }
    ctx.notify({Category::All});
    return Result::Finished;
  };
  register_operator(std::move(ot));
}

/* TODO(roadmap 3): "props.set": change any value by its path. Every number field uses it. */

void register_types()
{
  register_quit();
  register_undo();
  register_redo();

  Keymap &km = keymap("Window");
  km.add_key(Key::Q, {.ctrl = true}, "wm.quit");
  km.add_key(Key::Z, {.ctrl = true}, "edit.undo");
  km.add_key(Key::Z, {.ctrl = true, .shift = true}, "edit.redo");
}

}  // namespace wm
