#include <SDL3/SDL.h>

#include "platform/intern/sdl_event.hh"

namespace platform {

static Key key_from_sdl(SDL_Keycode key)
{
  if (key >= SDLK_A && key <= SDLK_Z) {
    return Key(int(Key::A) + int(key - SDLK_A));
  }
  switch (key) {
    case SDLK_ESCAPE: return Key::Escape;
    case SDLK_RETURN: return Key::Enter;
    case SDLK_SPACE: return Key::Space;
    case SDLK_TAB: return Key::Tab;
    case SDLK_BACKSPACE: return Key::Backspace;
    case SDLK_DELETE: return Key::Delete;
    default: return Key::Unknown;
  }
}

static MouseButton button_from_sdl(Uint8 button)
{
  switch (button) {
    case SDL_BUTTON_LEFT: return MouseButton::Left;
    case SDL_BUTTON_MIDDLE: return MouseButton::Middle;
    case SDL_BUTTON_RIGHT: return MouseButton::Right;
    default: return MouseButton::None;
  }
}

static Modifiers mods_from_sdl(SDL_Keymod mod)
{
  return {(mod & SDL_KMOD_CTRL) != 0, (mod & SDL_KMOD_SHIFT) != 0, (mod & SDL_KMOD_ALT) != 0};
}

Event event_from_sdl(const SDL_Event &ev)
{
  Event e;
  e.mods = mods_from_sdl(SDL_GetModState());
  switch (ev.type) {
    case SDL_EVENT_QUIT:
      e.type = EventType::Quit;
      break;
    case SDL_EVENT_WINDOW_CLOSE_REQUESTED:
      e.type = EventType::WindowClose;
      break;
    case SDL_EVENT_WINDOW_RESIZED:
    case SDL_EVENT_WINDOW_PIXEL_SIZE_CHANGED:
      e.type = EventType::WindowResize;
      break;
    case SDL_EVENT_MOUSE_MOTION:
      e.type = EventType::MouseMove;
      e.mouse = {ev.motion.x, ev.motion.y};
      break;
    case SDL_EVENT_MOUSE_BUTTON_DOWN:
    case SDL_EVENT_MOUSE_BUTTON_UP:
      e.type = ev.type == SDL_EVENT_MOUSE_BUTTON_DOWN ? EventType::MouseDown : EventType::MouseUp;
      e.mouse = {ev.button.x, ev.button.y};
      e.button = button_from_sdl(ev.button.button);
      break;
    case SDL_EVENT_MOUSE_WHEEL:
      e.type = EventType::MouseWheel;
      e.mouse = {ev.wheel.mouse_x, ev.wheel.mouse_y};
      e.wheel = {ev.wheel.x, ev.wheel.y};
      break;
    case SDL_EVENT_KEY_DOWN:
    case SDL_EVENT_KEY_UP:
      e.type = ev.type == SDL_EVENT_KEY_DOWN ? EventType::KeyDown : EventType::KeyUp;
      e.key = key_from_sdl(ev.key.key);
      e.repeat = ev.key.repeat;
      e.mods = mods_from_sdl(ev.key.mod);
      break;
    default:
      break;
  }
  return e;
}

const char *key_name(Key key)
{
  static char letter[2] = {0, 0};
  if (key >= Key::A && key <= Key::Z) {
    letter[0] = char('A' + (int(key) - int(Key::A)));
    return letter;
  }
  switch (key) {
    case Key::Escape: return "Esc";
    case Key::Enter: return "Enter";
    case Key::Space: return "Space";
    case Key::Tab: return "Tab";
    case Key::Backspace: return "Backspace";
    case Key::Delete: return "Delete";
    default: return "?";
  }
}

}  // namespace platform
