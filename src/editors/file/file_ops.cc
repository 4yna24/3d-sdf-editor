#include "editors/file/file_ops.hh"

#include "scene/document.hh"
#include "wm/keymap.hh"
#include "wm/menu.hh"
#include "wm/operator.hh"

namespace editors::file {

using namespace wm;

static void register_new()
{
  OperatorType ot;
  ot.id = "file.new";
  ot.label = "New";
  ot.description = "Start a new scene";
  ot.exec = [](Context &ctx, props::Values &) {
    /* TODO: ask first when there are unsaved changes; clear undo. */
    ctx.doc = scene::new_document();
    ctx.notify({Category::All});
    return Result::Finished;
  };
  register_operator(std::move(ot));
}

/* An operator that is listed in menus but cannot run yet (greyed out). */
static void register_later(const char *id, const char *label)
{
  OperatorType ot;
  ot.id = id;
  ot.label = label;
  ot.poll = [](const Context &) { return false; };
  ot.exec = [](Context &, props::Values &) { return Result::Cancelled; };
  register_operator(std::move(ot));
}

void register_operators()
{
  register_new();
  /* TODO(roadmap 9): file dialog from platform, then io::load_scene / io::save_scene. */
  register_later("file.open", "Open...");
  register_later("file.save", "Save");
  register_later("file.save_as", "Save As...");

  /* TODO(roadmap 9): one item per format: file.export_obj, file.export_stl, file.export_png. */
  register_menu({"TOPBAR_MT_file_export", "Export", {}});

  Keymap &km = keymap("Window");
  km.add_key(Key::N, {.ctrl = true}, "file.new");
  km.add_key(Key::O, {.ctrl = true}, "file.open");
  km.add_key(Key::S, {.ctrl = true}, "file.save");
  km.add_key(Key::S, {.ctrl = true, .shift = true}, "file.save_as");
}

}  // namespace editors::file
