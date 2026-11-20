# Draws the 3D view: draw engines (ray march, mesh) and overlays. See ai/rendering.md.
sdfe_add_module(draw
  SOURCES
    intern/engine.cc
    intern/gpu_scene.cc
    engines/raymarch/raymarch_engine.cc
    engines/mesh/mesh_engine.cc
    overlay/overlay.cc
  USES
    gpu
    scene
    sdf
    base
)
