#pragma once

#include "base/math.hh"

namespace gpu {

using GLProc = void (*)();
using GLLoader = GLProc (*)(const char *name);

/** Load the OpenGL functions. Call once, after the first window (and its context) exists. */
bool init(GLLoader loader);

/** For shaders: "#version 330 core". */
const char *glsl_version();

void set_viewport(int x, int y, int width, int height);
void clear(base::vec4 color);

}  // namespace gpu
