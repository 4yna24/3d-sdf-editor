#include "wm/keymap.hh"

#include <map>

namespace wm {

bool KeymapItem::matches(const Event &e) const
{
  if (e.type != type || e.mods != mods) {
    return false;
  }
  switch (type) {
    case EventType::KeyDown:
    case EventType::KeyUp:
      return e.key == key && !e.repeat;
    case EventType::MouseDown:
    case EventType::MouseUp:
      return e.button == button;
    default:
      return true;
  }
}

std::string KeymapItem::shortcut_text() const
{
  if (type != EventType::KeyDown) {
    return {};
  }
  std::string text;
  if (mods.ctrl) {
    text += "Ctrl+";
  }
  if (mods.shift) {
    text += "Shift+";
  }
  if (mods.alt) {
    text += "Alt+";
  }
  return text + platform::key_name(key);
}

KeymapItem &Keymap::add_key(Key key, Modifiers mods, std::string op)
{
  KeymapItem &item = items.emplace_back();
  item.type = EventType::KeyDown;
  item.key = key;
  item.mods = mods;
  item.op = std::move(op);
  return item;
}

KeymapItem &Keymap::add_mouse(MouseButton button, Modifiers mods, std::string op)
{
  KeymapItem &item = items.emplace_back();
  item.type = EventType::MouseDown;
  item.button = button;
  item.mods = mods;
  item.op = std::move(op);
  return item;
}

static std::map<std::string, Keymap> &registry()
{
  static std::map<std::string, Keymap> keymaps;
  return keymaps;
}

Keymap &keymap(const std::string &id)
{
  Keymap &km = registry()[id];
  km.id = id;
  return km;
}

const Keymap *find_keymap(const std::string &id)
{
  auto it = registry().find(id);
  return it == registry().end() ? nullptr : &it->second;
}

std::string shortcut_text(const std::string &op_id)
{
  for (const auto &[id, km] : registry()) {
    for (const KeymapItem &item : km.items) {
      if (item.op == op_id) {
        return item.shortcut_text();
      }
    }
  }
  return {};
}

}  // namespace wm
