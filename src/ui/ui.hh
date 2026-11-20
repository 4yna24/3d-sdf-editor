#pragma once

/* Start, stop and frame handling for the widget library.
 *
 * Order at startup:   ui::init() → window + platform::imgui_init() → gpu::init() → ui::init_renderer()
 * Order per frame:    platform::imgui_new_frame() → ui::begin_frame() → draw areas → ui::end_frame() */

namespace ui {

void init();          /* Create the ImGui context and apply the theme. */
void init_renderer(); /* After the OpenGL context exists. */
void shutdown_renderer();
void shutdown();

void begin_frame();
void end_frame(); /* Clear the window and draw every widget of this frame. */

/** Height of one header line (area headers, top bar). */
float header_height();

/* Asked by wm while sending events, see ai/code-structure.md "Event order". */
bool wants_mouse();    /* A widget is under the mouse or being dragged. */
bool wants_keyboard(); /* A text field has focus. */
bool popup_open();     /* A menu or popup is open. */

enum class Cursor { Arrow, ResizeEW, ResizeNS };
void set_cursor(Cursor cursor); /* For this frame. */

}  // namespace ui
