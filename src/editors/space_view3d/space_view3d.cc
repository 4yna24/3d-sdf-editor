#include "editors/space_view3d/space_view3d.hh"

#include "draw/engine.hh"
#include "ui/widgets.hh"
#include "wm/editor.hh"
#include "wm/menu.hh"

namespace editors::view3d {

/* Each viewport has its own state: mode now, camera later. See ai/ui.md, "3D Viewport".
 *
 * TODO(roadmap 5): orbit camera (camera.hh), a gpu::Framebuffer per viewport, the chosen
 *                  draw::Engine draws into it, ui::image() shows it.
 * TODO(roadmap 6): "view3d.select" in the View3D keymap (left click). */
class View3DEditor : public wm::Editor {
 public:
  void draw_header(wm::Context &ctx) override
  {
    wm::draw_menu(ctx, "VIEW3D_MT_view");
    wm::draw_menu(ctx, "VIEW3D_MT_add");

    /* Mode switch on the right: one button per registered draw engine. */
    float width = 0.0f;
    for (const draw::EngineType &t : draw::engine_types()) {
      width += ui::button_width(t.label);
    }
    ui::align_right(width);
    for (const draw::EngineType &t : draw::engine_types()) {
      if (ui::toggle(t.label, t.id == engine_)) {
        engine_ = t.id;
      }
    }
  }

  void draw_main(wm::Context &, wm::Rect) override
  {
    const draw::EngineType *engine = draw::find_engine_type(engine_);
    ui::centered_label(std::string("3D Viewport (") + (engine ? engine->label : "?") +
                       ")\ndrawing comes in roadmap step 5");
  }

  base::json save_state() const override
  {
    return {{"engine", engine_}};
  }

  void load_state(const base::json &state) override
  {
    engine_ = state.value("engine", engine_);
  }

 private:
  std::string engine_ = "RAYMARCH";
};

void register_type()
{
  wm::register_editor_type({
      .id = "VIEW_3D",
      .label = "3D Viewport",
      .icon = "3D",
      .keymap = "View3D",
      .create = [] { return std::make_unique<View3DEditor>(); },
  });

  /* TODO(roadmap 5): view.frame_selected, numpad views. TODO(roadmap 4): object.add per shape. */
  wm::register_menu({"VIEW3D_MT_view", "View", {}});
  wm::register_menu({"VIEW3D_MT_add", "Add", {}});
}

}  // namespace editors::view3d
