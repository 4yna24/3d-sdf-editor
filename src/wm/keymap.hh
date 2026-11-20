#pragma once

/* Hotkeys: tables of "key → operator id". One keymap per editor type, plus "Window".
 * TODO(later): editable in Preferences → Input. */

#include <string>
#include <vector>

#include "platform/event.hh"
#include "props/props.hh"

namespace wm {

using platform::Event;
using platform::EventType;
using platform::Key;
using platform::Modifiers;
using platform::MouseButton;

struct KeymapItem {
  EventType type = EventType::KeyDown;
  Key key = Key::Unknown;
  MouseButton button = MouseButton::None;
  Modifiers mods;
  std::string op;
  props::Values args;

  bool matches(const Event &e) const;
  std::string shortcut_text() const; /* "Ctrl+Z" */
};

struct Keymap {
  std::string id;
  std::vector<KeymapItem> items;

  KeymapItem &add_key(Key key, Modifiers mods, std::string op);
  KeymapItem &add_mouse(MouseButton button, Modifiers mods, std::string op);
};

/** The keymap with this id. Made empty the first time it is asked for. */
Keymap &keymap(const std::string &id);
const Keymap *find_keymap(const std::string &id);

/** The hotkey shown next to an operator in menus. Empty when it has none. */
std::string shortcut_text(const std::string &op_id);

}  // namespace wm
