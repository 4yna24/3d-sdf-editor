#pragma once

/* Talks to the OS through SDL3: windows, OpenGL context, events, time.
 * Later: file dialogs, clipboard. */

#include <string>

#include "base/math.hh"
#include "platform/event.hh"

namespace platform {

/** An OS window with an OpenGL context. What is inside is private to this module. */
struct Window;

bool init();
void shutdown();

Window *create_window(const std::string &title, int width, int height);
void destroy_window(Window *win);
base::vec2 window_size(const Window *win); /* In window units, same as mouse positions. */
void swap_buffers(Window *win);

/** Sleep until an event comes. `timeout_ms` < 0 waits forever. False on timeout. */
bool wait_event(Event &e, int timeout_ms);
/** Take the next waiting event without sleeping. False when there is none. */
bool poll_event(Event &e);

double time_seconds();

/** Gives gpu::init() the OpenGL functions, without gpu knowing about SDL. */
using GLProc = void (*)();
using GLLoader = GLProc (*)(const char *name);
GLLoader gl_loader();

/* Dear ImGui's SDL backend. Kept here so SDL stays inside this module.
 * The ImGui context must exist first (ui::init()). */
void imgui_init(Window *win);
void imgui_new_frame();
void imgui_shutdown();

}  // namespace platform
