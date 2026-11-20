#include "editors/space_properties/space_properties.hh"

#include "ui/widgets.hh"
#include "wm/editor.hh"

namespace editors::properties {

/* TODO(roadmap 4): registered tabs {id, icon, poll, draw}; contents drawn from the property
 *                  descriptions with ui::property(). See ai/ui.md, "Properties". */
class PropertiesEditor : public wm::Editor {
 public:
  void draw_main(wm::Context &, wm::Rect) override
  {
    ui::centered_label("Properties");
  }
};

void register_type()
{
  wm::register_editor_type({
      .id = "PROPERTIES",
      .label = "Properties",
      .icon = "PR",
      .keymap = "Properties",
      .create = [] { return std::make_unique<PropertiesEditor>(); },
  });
}

}  // namespace editors::properties
