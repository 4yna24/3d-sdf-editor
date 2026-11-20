# The only module that calls OpenGL. Replacing OpenGL (e.g. with Vulkan) changes only this module.
sdfe_add_module(gpu
  SOURCES
    intern/gpu.cc
  USES
    base
  LIBS
    glad_gl_core_33
)
