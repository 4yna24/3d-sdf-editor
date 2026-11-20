#pragma once

/* Widgets for editors. Every function draws into the region begun last.
 * TODO(roadmap 4): property(ctx, desc, value): the right widget for any property description. */

#include <string>

#include "base/rect.hh"

namespace ui {

/* Regions: the rectangles of an area. Calls between begin and end draw inside. */
void begin_header(const char *id, const base::Rect &rect); /* One line: menus and buttons. */
void end_header();
void begin_region(const char *id, const base::Rect &rect); /* The main part of an area. */
void end_region();

void label(const std::string &text);
void label_dim(const std::string &text);
void centered_label(const std::string &text); /* In the middle of the region. Placeholders. */
void same_line();

/** Move to the right edge so that `width` still fits. Use button_width() to measure. */
void align_right(float width);
float button_width(const std::string &label);

bool button(const std::string &label);
bool toggle(const std::string &label, bool active); /* A button that shows whether it is on. */

/* Menus. begin_menu() is true while the menu is open; call end_menu() only then. */
bool begin_menu(const std::string &label, bool enabled = true);
bool menu_item(const std::string &label,
               const std::string &shortcut = {},
               bool enabled = true,
               bool checked = false);
void menu_separator();
void end_menu();

}  // namespace ui
