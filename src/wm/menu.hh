#pragma once

/* Menus are lists of operator ids, registered at startup. Drawing them is written once, here. */

#include <string>
#include <vector>

#include "wm/context.hh"

namespace wm {

struct MenuType {
  std::string id;    /* "TOPBAR_MT_file" (Blender's naming) */
  std::string label; /* "File" */
  /** Operator ids. "-" is a separator, "menu:<id>" a submenu. */
  std::vector<std::string> items;
};

void register_menu(MenuType menu);
const MenuType *find_menu(const std::string &id);

/** Draw the menu button and, while it is open, its items. Clicking an item runs its operator. */
void draw_menu(Context &ctx, const std::string &id);

}  // namespace wm
