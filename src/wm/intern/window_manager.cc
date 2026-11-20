#include "wm/window_manager.hh"

#include <algorithm>

#include "base/log.hh"
#include "ui/widgets.hh"
#include "wm/keymap.hh"

namespace wm {

/* After an event, draw a few frames: ImGui needs them to settle hover and click states. */
static constexpr int FRAMES_AFTER_EVENT = 3;

/* The editor type every window shows in its top bar. */
static const char *TOPBAR_TYPE = "TOPBAR";

WindowManager::WindowManager(scene::Document &doc, prefs::Preferences &prefs)
    : doc_(doc), prefs_(prefs)
{
}

WindowManager::~WindowManager()
{
  close_windows();
}

int WindowManager::add_workspace(std::unique_ptr<Workspace> workspace)
{
  workspaces_.push_back(std::move(workspace));
  return int(workspaces_.size()) - 1;
}

Window *WindowManager::open_window(const std::string &title, int workspace)
{
  platform::Window *native = platform::create_window(title, 1280, 800);
  if (!native) {
    return nullptr;
  }
  if (windows_.empty()) {
    platform::imgui_init(native); /* TODO: more windows (Preferences) need more ImGui setup. */
  }
  auto win = std::make_unique<Window>();
  win->native = native;
  win->workspace = workspace;
  win->topbar = new_area(TOPBAR_TYPE);
  win->topbar->global = true;
  windows_.push_back(std::move(win));
  redraw_frames_ = FRAMES_AFTER_EVENT;
  return windows_.back().get();
}

void WindowManager::close_windows()
{
  if (windows_.empty()) {
    return;
  }
  platform::imgui_shutdown();
  for (auto &win : windows_) {
    platform::destroy_window(win->native);
  }
  windows_.clear();
}

void WindowManager::quit()
{
  quit_ = true;
}

Context WindowManager::context(Window *win, Area *area)
{
  return Context{*this, win, area, doc_, prefs_};
}

Screen *WindowManager::screen(Window &win)
{
  if (win.workspace < 0 || win.workspace >= int(workspaces_.size())) {
    return nullptr;
  }
  return &workspaces_[win.workspace]->screen;
}

void WindowManager::notify(const Notifier &n)
{
  notifiers_.push_back(n);
}

/* ------------------------------------------------------------------------------------------ */
/* Main loop. See ai/code-structure.md, "One pass of the main loop". */

void WindowManager::run()
{
  while (!quit_ && !windows_.empty()) {
    /* 1. Wait for events. Sleep while nothing needs drawing. */
    platform::Event e;
    if (redraw_frames_ == 0 && platform::wait_event(e, -1)) {
      handle_event(e);
    }
    /* 2. Send each event to a handler. */
    while (!quit_ && platform::poll_event(e)) {
      handle_event(e);
    }
    /* 3. Collect finished background jobs. */
    if (jobs_.collect_finished()) {
      redraw_frames_ = FRAMES_AFTER_EVENT;
    }
    /* 4. Give the notifiers to every editor. */
    send_notifiers();
    /* 5. Draw. Operators started from widgets run right after the frame. */
    if (redraw_frames_ > 0 && !quit_) {
      redraw_frames_--;
      for (auto &win : windows_) {
        draw(*win);
      }
      run_queued();
      send_notifiers();
    }
  }
}

/* ------------------------------------------------------------------------------------------ */
/* Events. See ai/code-structure.md, "Event order". */

void WindowManager::handle_event(const Event &e)
{
  redraw_frames_ = FRAMES_AFTER_EVENT;
  if (e.type == EventType::None) {
    return;
  }
  if (e.type == EventType::Quit || e.type == EventType::WindowClose) {
    quit();
    return;
  }
  if (platform::is_mouse_event(e)) {
    mouse_ = e.mouse;
  }

  Window &win = *windows_.front(); /* TODO: the window the event belongs to. */
  Area *area = area_at(win, mouse_);
  Context ctx = context(&win, area);
  current_window_ = &win;
  current_area_ = area;

  [&] {
    /* 1. A running modal operator gets everything. */
    if (modal_.type) {
      Context mctx = context(modal_.window, find_area(*modal_.window, modal_.area_id));
      const Result r = modal_.type->modal(mctx, modal_.args, e);
      if (r == Result::Running) {
        return;
      }
      if (r != Result::PassThrough) { /* Finished or Cancelled: the modal operator ends. */
        if (r == Result::Finished && modal_.type->undo) {
          undo_.push(doc_.scene, modal_.type->label);
        }
        modal_ = {};
        return;
      }
    }
    /* 2. An open menu or popup: ImGui handles it. */
    if (ui::popup_open()) {
      return;
    }
    /* 3. Area edges. */
    if (e.type == EventType::MouseDown && e.button == MouseButton::Left) {
      Screen *screen = this->screen(win);
      if (screen && screen->split_at(e.mouse)) {
        if (const OperatorType *ot = find_operator("screen.area_move")) {
          props::Values args;
          call_operator(*ot, ctx, args, &e);
          return;
        }
      }
    }
    /* 4. The area under the mouse. */
    if (area) {
      const bool widget = platform::is_mouse_event(e) ? ui::wants_mouse() : ui::wants_keyboard();
      if (widget) {
        return; /* a. A widget uses it. */
      }
      const EditorType *type = find_editor_type(area->editor_type);
      if (type && run_keymap(type->keymap, ctx, e)) {
        return; /* b. The editor's keymap. */
      }
      if (area->editor && area->editor->handle_event(ctx, e)) {
        return; /* c. The editor itself. */
      }
    }
    /* 5. The window keymap. */
    run_keymap("Window", ctx, e);
  }();

  current_window_ = nullptr;
  current_area_ = nullptr;
}

bool WindowManager::run_keymap(const std::string &keymap_id, Context &ctx, const Event &e)
{
  const Keymap *km = find_keymap(keymap_id);
  if (!km) {
    return false;
  }
  for (const KeymapItem &item : km->items) {
    if (!item.matches(e)) {
      continue;
    }
    const OperatorType *ot = find_operator(item.op);
    if (!ot || (ot->poll && !ot->poll(ctx))) {
      continue;
    }
    props::Values args = item.args;
    if (call_operator(*ot, ctx, args, &e) != Result::PassThrough) {
      return true;
    }
  }
  return false;
}

/* ------------------------------------------------------------------------------------------ */
/* Operators. */

Result WindowManager::run_operator(const std::string &id, props::Values args)
{
  const OperatorType *ot = find_operator(id);
  if (!ot) {
    base::log_warning("Unknown operator '{}'", id);
    return Result::Cancelled;
  }
  if (drawing_) {
    queued_.push_back({id, std::move(args), current_window_, current_area_ ? current_area_->id : 0});
    return Result::Running;
  }
  Context ctx = context(current_window_, current_area_);
  return call_operator(*ot, ctx, args, nullptr);
}

Result WindowManager::call_operator(const OperatorType &ot,
                                    Context &ctx,
                                    props::Values &args,
                                    const Event *e)
{
  if (ot.poll && !ot.poll(ctx)) {
    return Result::Cancelled;
  }
  Result r = Result::Cancelled;
  if (e && ot.invoke) {
    r = ot.invoke(ctx, args, *e);
  }
  else if (ot.exec) {
    r = ot.exec(ctx, args);
  }

  if (r == Result::Running) {
    if (ot.modal && ctx.window) {
      modal_ = {&ot, args, ctx.window, ctx.area ? ctx.area->id : 0};
    }
    else {
      r = Result::Cancelled;
    }
  }
  if (r == Result::Finished) {
    if (ot.undo) {
      undo_.push(doc_.scene, ot.label);
    }
    redraw_frames_ = FRAMES_AFTER_EVENT;
  }
  return r;
}

void WindowManager::run_queued()
{
  std::vector<Queued> queued = std::move(queued_);
  queued_.clear();
  for (Queued &q : queued) {
    const OperatorType *ot = find_operator(q.op);
    if (!ot) {
      continue;
    }
    Context ctx = context(q.window, q.window ? find_area(*q.window, q.area_id) : nullptr);
    call_operator(*ot, ctx, q.args, nullptr);
  }
}

/* ------------------------------------------------------------------------------------------ */
/* Notifiers. */

void WindowManager::send_notifiers()
{
  if (notifiers_.empty()) {
    return;
  }
  auto send = [&](Area &area) {
    if (!area.editor) {
      return;
    }
    for (const Notifier &n : notifiers_) {
      if (area.editor->on_notify(n)) {
        area.needs_redraw = true;
      }
    }
    if (area.needs_redraw) {
      redraw_frames_ = FRAMES_AFTER_EVENT;
    }
  };
  for (auto &win : windows_) {
    send(*win->topbar);
    if (Screen *screen = this->screen(*win)) {
      screen->for_each_area(send);
    }
  }
  notifiers_.clear();
}

/* ------------------------------------------------------------------------------------------ */
/* Drawing. ImGui redraws the whole window, so one area needing it means the window is drawn. */

void WindowManager::draw(Window &win)
{
  drawing_ = true;
  current_window_ = &win;
  platform::imgui_new_frame();
  ui::begin_frame();

  /* Layout: the top bar, then the screen fills the rest. */
  const base::vec2 size = platform::window_size(win.native);
  const float header_height = ui::header_height();
  win.topbar->rect = {0.0f, 0.0f, size.x, header_height};
  win.topbar->header = win.topbar->rect;
  win.topbar->main = {};
  Screen *screen = this->screen(win);
  if (screen) {
    screen->layout({0.0f, header_height, size.x, size.y - header_height}, header_height);
  }

  draw_area(win, *win.topbar);
  if (screen) {
    screen->for_each_area([&](Area &area) { draw_area(win, area); });
  }

  /* Resize cursor over area edges (kept while dragging one). */
  if (!modal_.type) {
    const ScreenNode *split = screen ? screen->split_at(mouse_) : nullptr;
    cursor_ = !split                              ? ui::Cursor::Arrow
              : split->dir == SplitDir::LeftRight ? ui::Cursor::ResizeEW
                                                  : ui::Cursor::ResizeNS;
  }
  if (cursor_ != ui::Cursor::Arrow) {
    ui::set_cursor(cursor_);
  }

  ui::end_frame();
  platform::swap_buffers(win.native);
  current_window_ = nullptr;
  drawing_ = false;
}

void WindowManager::draw_area(Window &win, Area &area)
{
  current_area_ = &area;
  Context ctx = context(&win, &area);
  const std::string id = "##area" + std::to_string(area.id);

  /* Header: editor type button, then the editor's own part. */
  ui::begin_header((id + "_header").c_str(), area.header);
  if (!area.global) {
    draw_editor_type_button(ctx, area);
  }
  if (area.editor) {
    area.editor->draw_header(ctx);
  }
  ui::end_header();

  /* Main region. The top bar has none. */
  if (!area.global) {
    ui::begin_region((id + "_main").c_str(), area.main);
    if (area.editor) {
      area.editor->draw_main(ctx, area.main);
    }
    else {
      ui::centered_label("Unknown editor type: " + area.editor_type);
    }
    ui::end_region();
  }

  area.needs_redraw = false;
  current_area_ = nullptr;
}

void WindowManager::draw_editor_type_button(Context &ctx, Area &area)
{
  const EditorType *current = find_editor_type(area.editor_type);
  const std::string label = (current ? current->icon : std::string("?")) + "##editor_type";
  if (!ui::begin_menu(label)) {
    return;
  }
  for (const EditorType &type : editor_types()) {
    if (!type.in_type_menu) {
      continue;
    }
    if (ui::menu_item(type.label, {}, true, type.id == area.editor_type)) {
      props::Values args;
      args.set("type", type.id);
      ctx.wm.run_operator("screen.space_type_set", std::move(args));
    }
  }
  ui::end_menu();
}

Area *WindowManager::area_at(Window &win, base::vec2 p)
{
  if (win.topbar && win.topbar->rect.contains(p)) {
    return win.topbar.get();
  }
  Screen *screen = this->screen(win);
  return screen ? screen->area_at(p) : nullptr;
}

Area *WindowManager::find_area(Window &win, int area_id)
{
  if (win.topbar && win.topbar->id == area_id) {
    return win.topbar.get();
  }
  Area *found = nullptr;
  if (Screen *screen = this->screen(win)) {
    screen->for_each_area([&](Area &a) {
      if (a.id == area_id) {
        found = &a;
      }
    });
  }
  return found;
}

}  // namespace wm
