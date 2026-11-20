# The data: document, scene, objects, shape types. No UI, no OpenGL.
sdfe_add_module(scene
  SOURCES
    intern/scene.cc
    intern/document.cc
    intern/shape_type.cc
    intern/sdf_program.cc
    shapes/sphere.cc
    shapes/box.cc
  USES
    sdf
    props
    base
)

sdfe_add_tests(scene SOURCES tests/test_scene.cc)
