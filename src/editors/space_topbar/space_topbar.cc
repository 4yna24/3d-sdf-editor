#include "editors/space_topbar/space_topbar.hh"

#include "ui/widgets.hh"
#include "wm/editor.hh"
#include "wm/menu.hh"
#include "wm/window_manager.hh"

namespace editors::topbar {

class TopbarEditor : public wm::Editor {
 public:
  void draw_header(wm::Context &ctx) override
  {
    wm::draw_menu(ctx, "TOPBAR_MT_file");
    wm::draw_menu(ctx, "TOPBAR_MT_edit");
    ui::label_dim("|");

    /* Workspace tabs. TODO(later): an operator that switches ctx.window->workspace. */
    const auto &workspaces = ctx.wm.workspaces();
    for (int i = 0; i < int(workspaces.size()); i++) {
      const bool active = ctx.window && ctx.window->workspace == i;
      ui::toggle(workspaces[i]->name, active);
    }
  }

  void draw_main(wm::Context &, wm::Rect) override {} /* The top bar is header only. */
};

void register_type()
{
  wm::register_editor_type({
      .id = "TOPBAR",
      .label = "Top Bar",
      .icon = "TB",
      .keymap = "",
      .create = [] { return std::make_unique<TopbarEditor>(); },
      .in_type_menu = false,
  });

  /* Items are operator ids. "-" is a separator, "menu:<id>" a submenu. */
  wm::register_menu({"TOPBAR_MT_file",
                     "File",
                     {"file.new",
                      "file.open",
                      "-",
                      "file.save",
                      "file.save_as",
                      "-",
                      "menu:TOPBAR_MT_file_export",
                      "-",
                      "wm.quit"}});
  wm::register_menu({"TOPBAR_MT_edit", "Edit", {"edit.undo", "edit.redo", "-", "preferences.show"}});
}

}  // namespace editors::topbar
