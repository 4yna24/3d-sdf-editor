#include "wm/menu.hh"

#include <map>

#include "ui/widgets.hh"
#include "wm/keymap.hh"
#include "wm/operator.hh"
#include "wm/window_manager.hh"

namespace wm {

static std::map<std::string, MenuType> &registry()
{
  static std::map<std::string, MenuType> menus;
  return menus;
}

void register_menu(MenuType menu)
{
  std::string id = menu.id;
  registry()[id] = std::move(menu);
}

const MenuType *find_menu(const std::string &id)
{
  auto it = registry().find(id);
  return it == registry().end() ? nullptr : &it->second;
}

static void draw_item(Context &ctx, const std::string &item)
{
  if (item == "-") {
    ui::menu_separator();
    return;
  }
  if (item.starts_with("menu:")) {
    draw_menu(ctx, item.substr(5));
    return;
  }
  const OperatorType *ot = find_operator(item);
  if (!ot) {
    ui::menu_item(item, {}, false); /* Not written yet: show its id, greyed out. */
    return;
  }
  const bool enabled = !ot->poll || ot->poll(ctx);
  if (ui::menu_item(ot->label, shortcut_text(ot->id), enabled)) {
    ctx.wm.run_operator(ot->id);
  }
}

void draw_menu(Context &ctx, const std::string &id)
{
  const MenuType *menu = find_menu(id);
  if (!menu) {
    return;
  }
  if (ui::begin_menu(menu->label, !menu->items.empty())) {
    for (const std::string &item : menu->items) {
      draw_item(ctx, item);
    }
    ui::end_menu();
  }
}

}  // namespace wm
