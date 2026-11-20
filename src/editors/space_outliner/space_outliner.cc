#include "editors/space_outliner/space_outliner.hh"

#include "ui/widgets.hh"
#include "wm/editor.hh"

namespace editors::outliner {

/* TODO(roadmap 4): draw ctx.scene().root as a tree; click selects, double click renames,
 *                  eye icon hides, right click menu (Delete, Duplicate, New Collection). */
class OutlinerEditor : public wm::Editor {
 public:
  void draw_main(wm::Context &, wm::Rect) override
  {
    ui::centered_label("Outliner");
  }

  bool on_notify(const wm::Notifier &n) override
  {
    /* Only names, the tree and the selection matter here; not shape values. */
    return n.category != wm::Category::Object || n.action != wm::Action::Changed;
  }
};

void register_type()
{
  wm::register_editor_type({
      .id = "OUTLINER",
      .label = "Outliner",
      .icon = "OL",
      .keymap = "Outliner",
      .create = [] { return std::make_unique<OutlinerEditor>(); },
  });
}

}  // namespace editors::outliner
