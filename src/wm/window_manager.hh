#pragma once

/* Windows, workspaces, the main loop and sending events to the right place. */

#include <memory>
#include <string>
#include <vector>

#include "platform/platform.hh"
#include "props/props.hh"
#include "ui/ui.hh"
#include "wm/context.hh"
#include "wm/jobs.hh"
#include "wm/notifier.hh"
#include "wm/operator.hh"
#include "wm/screen.hh"
#include "wm/undo.hh"

namespace wm {

/** One OS window: the top bar, and below it the screen of one workspace. */
struct Window {
  platform::Window *native = nullptr;
  std::unique_ptr<Area> topbar;
  int workspace = 0;
};

class WindowManager {
 public:
  WindowManager(scene::Document &doc, prefs::Preferences &prefs);
  ~WindowManager();

  /** Returns the index of the new workspace. */
  int add_workspace(std::unique_ptr<Workspace> workspace);
  const std::vector<std::unique_ptr<Workspace>> &workspaces() const
  {
    return workspaces_;
  }

  /** Open an OS window that shows a workspace. Null when it fails. */
  Window *open_window(const std::string &title, int workspace);
  void close_windows();

  /** The main loop. Returns after quit(). */
  void run();
  void quit();

  /** Run an operator with the current window and area.
   * While drawing, the call is queued and runs right after the frame (returns Running). */
  Result run_operator(const std::string &id, props::Values args = {});
  void notify(const Notifier &n);

  Context context(Window *win, Area *area);
  Screen *screen(Window &win);
  UndoStack &undo_stack()
  {
    return undo_;
  }

 private:
  struct Queued {
    std::string op;
    props::Values args;
    Window *window;
    int area_id;
  };
  struct Modal {
    const OperatorType *type = nullptr;
    props::Values args;
    Window *window = nullptr;
    int area_id = 0;
  };

  void handle_event(const Event &e);
  bool run_keymap(const std::string &keymap_id, Context &ctx, const Event &e);
  Result call_operator(const OperatorType &ot, Context &ctx, props::Values &args, const Event *e);
  void run_queued();
  void send_notifiers();
  void draw(Window &win);
  void draw_area(Window &win, Area &area);
  void draw_editor_type_button(Context &ctx, Area &area);
  Area *area_at(Window &win, base::vec2 p);
  Area *find_area(Window &win, int area_id);

  scene::Document &doc_;
  prefs::Preferences &prefs_;
  std::vector<std::unique_ptr<Workspace>> workspaces_;
  std::vector<std::unique_ptr<Window>> windows_;
  UndoStack undo_;
  Jobs jobs_;
  std::vector<Notifier> notifiers_;
  std::vector<Queued> queued_;
  Modal modal_;

  bool quit_ = false;
  bool drawing_ = false;
  int redraw_frames_ = 0; /* Frames still to draw. 0 = sleep until the next event. */
  base::vec2 mouse_{0.0f};
  ui::Cursor cursor_ = ui::Cursor::Arrow;
  /* Where run_operator() runs. Only set during one event or one area draw. */
  Window *current_window_ = nullptr;
  Area *current_area_ = nullptr;
};

}  // namespace wm
