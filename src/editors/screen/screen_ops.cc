#include "editors/screen/screen_ops.hh"

#include "ui/ui.hh"
#include "wm/keymap.hh"
#include "wm/operator.hh"
#include "wm/screen.hh"

namespace editors::screen {

using namespace wm;

/* Drag the edge between two areas. Started by wm when the mouse is pressed on an edge. */
static void register_area_move()
{
  OperatorType ot;
  ot.id = "screen.area_move";
  ot.label = "Move Area Edge";
  ot.poll = [](const Context &ctx) { return ctx.window != nullptr; };
  ot.invoke = [](Context &ctx, props::Values &args, const Event &e) {
    Screen *screen = ctx.screen();
    ScreenNode *split = screen ? screen->split_at(e.mouse) : nullptr;
    if (!split) {
      return Result::PassThrough;
    }
    args.set("split", split->id);
    args.set("start_ratio", split->ratio);
    return Result::Running;
  };
  ot.modal = [](Context &ctx, props::Values &args, const Event &e) {
    ScreenNode *split = ctx.screen() ? ctx.screen()->find_node(args.get<int>("split")) : nullptr;
    if (!split) {
      return Result::Cancelled;
    }
    switch (e.type) {
      case EventType::MouseMove:
        split->ratio = split_ratio_at(*split, e.mouse, ui::header_height() * 2.0f);
        ctx.notify({Category::Screen});
        return Result::Running;
      case EventType::MouseUp:
        return e.button == MouseButton::Left ? Result::Finished : Result::Running;
      case EventType::KeyDown:
        if (e.key == Key::Escape) {
          split->ratio = args.get<float>("start_ratio");
          ctx.notify({Category::Screen});
          return Result::Cancelled;
        }
        return Result::Running;
      default:
        return Result::Running;
    }
  };
  register_operator(std::move(ot));
}

/* Show another editor type in the area. Used by the editor type button in every area header. */
static void register_space_type_set()
{
  OperatorType ot;
  ot.id = "screen.space_type_set";
  ot.label = "Change Editor Type";
  ot.args.add_string("type", "Type");
  ot.poll = [](const Context &ctx) { return ctx.area && !ctx.area->global; };
  ot.exec = [](Context &ctx, props::Values &args) {
    ctx.area->set_editor_type(args.get<std::string>("type"));
    ctx.notify({Category::Screen});
    return Result::Finished;
  };
  register_operator(std::move(ot));
}

void register_operators()
{
  register_area_move();
  register_space_type_set();
}

}  // namespace editors::screen
