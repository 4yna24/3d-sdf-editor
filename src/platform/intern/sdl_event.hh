#pragma once

#include <SDL3/SDL_events.h>

#include "platform/event.hh"

namespace platform {

/** SDL event → our event. Events we do not use become EventType::None. */
Event event_from_sdl(const SDL_Event &ev);

}  // namespace platform
