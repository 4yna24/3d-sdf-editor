#include "editors/space_preferences/space_preferences.hh"

#include "ui/widgets.hh"
#include "wm/editor.hh"
#include "wm/operator.hh"

namespace editors::preferences {

/* TODO(roadmap 8): section list on the left, values drawn from prefs::describe() on the right. */
class PreferencesEditor : public wm::Editor {
 public:
  void draw_main(wm::Context &, wm::Rect) override
  {
    ui::centered_label("Preferences");
  }
};

void register_type()
{
  wm::register_editor_type({
      .id = "PREFERENCES",
      .label = "Preferences",
      .icon = "PF",
      .keymap = "",
      .create = [] { return std::make_unique<PreferencesEditor>(); },
  });

  wm::OperatorType ot;
  ot.id = "preferences.show";
  ot.label = "Preferences...";
  ot.description = "Open the Preferences window";
  ot.poll = [](const wm::Context &) { return false; }; /* TODO(roadmap 8): second OS window. */
  ot.exec = [](wm::Context &, props::Values &) { return wm::Result::Cancelled; };
  wm::register_operator(std::move(ot));
}

}  // namespace editors::preferences
