#pragma once

/* Our own input events. SDL events are turned into these, so no other module sees SDL. */

#include "base/math.hh"

namespace platform {

enum class EventType {
  None, /* Something SDL reported that we do not use. Still causes a redraw. */
  Quit,
  WindowClose,
  WindowResize,
  MouseMove,
  MouseDown,
  MouseUp,
  MouseWheel,
  KeyDown,
  KeyUp,
};

enum class MouseButton { None, Left, Middle, Right };

enum class Key {
  Unknown,
  Escape, Enter, Space, Tab, Backspace, Delete,
  A, B, C, D, E, F, G, H, I, J, K, L, M, N, O, P, Q, R, S, T, U, V, W, X, Y, Z,
};

struct Modifiers {
  bool ctrl = false;
  bool shift = false;
  bool alt = false;
  bool operator==(const Modifiers &) const = default;
};

struct Event {
  EventType type = EventType::None;
  base::vec2 mouse{0.0f}; /* Mouse position in the window. */
  base::vec2 wheel{0.0f};
  MouseButton button = MouseButton::None;
  Key key = Key::Unknown;
  bool repeat = false; /* Key held down. */
  Modifiers mods;
};

inline bool is_mouse_event(const Event &e)
{
  return e.type == EventType::MouseMove || e.type == EventType::MouseDown ||
         e.type == EventType::MouseUp || e.type == EventType::MouseWheel;
}

/** Text for menus and tooltips: "A", "Esc", ... */
const char *key_name(Key key);

}  // namespace platform
