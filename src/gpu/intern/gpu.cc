#include "gpu/gpu.hh"

#include <glad/gl.h>

#include "base/log.hh"

namespace gpu {

bool init(GLLoader loader)
{
  const int version = gladLoadGL(reinterpret_cast<GLADloadfunc>(loader));
  if (version == 0) {
    base::log_error("Could not load the OpenGL functions");
    return false;
  }
  base::log_info("OpenGL {}.{} on {}",
                 GLAD_VERSION_MAJOR(version),
                 GLAD_VERSION_MINOR(version),
                 reinterpret_cast<const char *>(glGetString(GL_RENDERER)));
  return true;
}

const char *glsl_version()
{
  return "#version 330 core";
}

void set_viewport(int x, int y, int width, int height)
{
  glViewport(x, y, width, height);
}

void clear(base::vec4 color)
{
  glClearColor(color.r, color.g, color.b, color.a);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

}  // namespace gpu
