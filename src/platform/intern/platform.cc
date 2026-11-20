#include "platform/platform.hh"

#include <SDL3/SDL.h>
#include <imgui_impl_sdl3.h>

#include "base/log.hh"
#include "platform/intern/sdl_event.hh"

namespace platform {

struct Window {
  SDL_Window *sdl = nullptr;
  SDL_GLContext gl = nullptr;
};

static bool g_imgui_ready = false;

bool init()
{
  if (!SDL_Init(SDL_INIT_VIDEO)) {
    base::log_error("SDL_Init failed: {}", SDL_GetError());
    return false;
  }
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
  SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
  SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
  return true;
}

void shutdown()
{
  SDL_Quit();
}

Window *create_window(const std::string &title, int width, int height)
{
  const SDL_WindowFlags flags = SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE |
                                SDL_WINDOW_HIGH_PIXEL_DENSITY;
  SDL_Window *sdl = SDL_CreateWindow(title.c_str(), width, height, flags);
  if (!sdl) {
    base::log_error("SDL_CreateWindow failed: {}", SDL_GetError());
    return nullptr;
  }
  /* TODO: share one OpenGL context between all windows when Preferences opens a second one. */
  SDL_GLContext gl = SDL_GL_CreateContext(sdl);
  if (!gl) {
    base::log_error("Could not create an OpenGL 3.3 context: {}", SDL_GetError());
    SDL_DestroyWindow(sdl);
    return nullptr;
  }
  SDL_GL_MakeCurrent(sdl, gl);
  SDL_GL_SetSwapInterval(1);
  return new Window{sdl, gl};
}

void destroy_window(Window *win)
{
  if (!win) {
    return;
  }
  SDL_GL_DestroyContext(win->gl);
  SDL_DestroyWindow(win->sdl);
  delete win;
}

base::vec2 window_size(const Window *win)
{
  int w = 0, h = 0;
  SDL_GetWindowSize(win->sdl, &w, &h);
  return {float(w), float(h)};
}

void swap_buffers(Window *win)
{
  SDL_GL_SwapWindow(win->sdl);
}

/* ImGui sees every SDL event first, then we turn it into our own. */
static Event translate(const SDL_Event &ev)
{
  if (g_imgui_ready) {
    ImGui_ImplSDL3_ProcessEvent(&ev);
  }
  return event_from_sdl(ev);
}

bool wait_event(Event &e, int timeout_ms)
{
  SDL_Event ev;
  if (!SDL_WaitEventTimeout(&ev, timeout_ms)) {
    return false;
  }
  e = translate(ev);
  return true;
}

bool poll_event(Event &e)
{
  SDL_Event ev;
  if (!SDL_PollEvent(&ev)) {
    return false;
  }
  e = translate(ev);
  return true;
}

double time_seconds()
{
  return double(SDL_GetTicksNS()) / 1e9;
}

GLLoader gl_loader()
{
  return [](const char *name) -> GLProc { return SDL_GL_GetProcAddress(name); };
}

void imgui_init(Window *win)
{
  ImGui_ImplSDL3_InitForOpenGL(win->sdl, win->gl);
  g_imgui_ready = true;
}

void imgui_new_frame()
{
  ImGui_ImplSDL3_NewFrame();
}

void imgui_shutdown()
{
  if (g_imgui_ready) {
    ImGui_ImplSDL3_Shutdown();
    g_imgui_ready = false;
  }
}

}  // namespace platform
