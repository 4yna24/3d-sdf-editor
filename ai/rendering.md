# Rendering

Where the code is: SDF math and the mesher in `sdf/`, viewport drawing in `draw/`, OpenGL in `gpu/`,
the viewport camera in `editors/space_view3d/`. See [code-structure.md](code-structure.md).

## Two viewport modes

| Mode | How it works | Good for |
|---|---|---|
| **Ray march** | A fragment shader sends one ray per pixel and steps along the SDF. Exact shape, smooth blends. | Default. Shows the real SDF. |
| **Mesh** | The SDF is turned into triangles once, then drawn the normal OpenGL way. | Weak GPUs; checking what an export will look like. |

Both read the same data and use the same lighting rules, so they look almost the same.

```
                      Scene
                        │  scene::sdf_program()   (cached, only changed objects rebuilt)
                        ▼
          SDF program: flat list of primitives
               │                         │
     upload to a GPU buffer       mesher jobs (background threads)
               │                         │
       ray march shader             mesh shader
               └────────────┬────────────┘
                 viewport image (one per viewport)
                            │
                 shown in the area by ui::image()
```

Each mode is a **draw engine**. New modes (path tracer, Vulkan) are new engines:

```cpp
// draw/engine.hh
class Engine {
 public:
  virtual ~Engine() = default;
  virtual void sync(const scene::Scene &) = 0;                   // pick up changes, by version numbers
  virtual void draw(const View &, gpu::Framebuffer &target) = 0; // draw into the viewport image
};
```

`View` holds the view matrix, the projection matrix, the image size and the shading settings.
The viewport camera makes it.

## One SDF library for GPU and CPU

All formulas (shapes and combine operators) live in **one file**, `sdf/glsl/sdf_library.glsl`. It is written
so that it also compiles as C++ with GLM (same names: `vec3`, `length`, `max`, ...). It also holds the
`switch` that picks the formula for a primitive type.

- The GPU shaders `#include` it.
- The CPU uses it for picking, the mesher, and the CPU fallback renderer.
- So the GPU and the CPU always see the same shape.

The formulas come from Inigo Quilez's articles and hg_sdf. The file says where each one comes from.

## The SDF program

`scene::sdf_program(scene)` turns the visible SDF objects into a flat list. One entry per object:

```
type, combine op, blend k, params[8], inverse transform matrix, color, object id, bounding sphere
```

The C++ struct has the same memory layout as the GPU buffer, so it is uploaded as is.
Only the entries of objects whose version changed are rebuilt.

## Ray march mode

1. The SDF program is uploaded to a GPU buffer. When one object changes, only its entry is uploaded again.
2. The shader's `map(p)` loops over the entries. For each: move `p` into the object's own space with the
   inverse matrix, compute the distance with the formula for its type, combine with the result so far.
3. Changing a number only updates the buffer. The shader is never recompiled for that.
4. Each entry has a bounding sphere. Far from it, the cheap sphere distance is used instead of the formula.
5. Later, for many objects: a BVH over the bounding boxes, or generating shader code for the scene and
   recompiling only when objects are added or removed.

The shader also writes **depth**, so the grid and the light and camera icons (normal lines) are hidden
correctly behind SDF objects.

## Mesh mode

- The **mesher** (`sdf/`) samples the SDF on a 3D grid and builds triangles. Use **Surface Nets**
  (nicer triangles) or **Marching Cubes**, whichever is simpler to get working first.
- Each object is meshed **in its own space**. Moving or rotating it only changes its matrix, with no new mesh.
  Only a change of shape values makes a new mesh.
- Objects that are joined with smooth union or subtract (later) are meshed **together**, because their
  surfaces merge.
- Meshing runs in **background jobs**. Until a new mesh is ready, the old one (or a coarse one) is shown.
- The grid resolution is a preference (Viewport). Export uses a finer grid.
- Normals come from the SDF gradient, not from the triangles, so the lighting is smooth.

## Viewport image and drawing only on change

- Each viewport draws into its **own image** (an offscreen framebuffer) the size of its area.
- If nothing changed (no notifier, no new version, camera did not move), the old image is shown again.
  Nothing is rendered.
- While the camera moves: lower resolution and fewer ray steps, no shadows. Full quality once it stops.

## GPU or no GPU

At startup the app checks what is available:

| Situation | What happens |
|---|---|
| GPU with OpenGL 3.3 or newer | Normal: both modes on the GPU. |
| No real GPU, but software OpenGL (Mesa llvmpipe, common on Linux) | Works, but slowly. Ray march runs at lower resolution; Mesh mode is suggested. |
| No OpenGL at all | A **CPU ray march engine** (multithreaded) draws into an image that is shown in the area. Same SDF library. |

The user can force a choice in Preferences → System.

## Viewport camera

The viewport camera is **not a scene object**. Every 3D Viewport editor has its own one, saved with the
layout, not with the scene. A **Camera object** in the scene is a different thing, used later for final
renders and "look through camera" (Numpad 0).

It is an orbit camera, as in Blender:

```
ViewportCamera
 ├─ target       the point it orbits around
 ├─ distance     from the target
 ├─ yaw, pitch   angles around the target
 ├─ fov
 └─ ortho        orthographic or not (Numpad 5)
```

| Input | Action (each one is a `view3d.*` operator) |
|---|---|
| Middle mouse drag | Orbit around the target |
| Shift + middle mouse | Pan (moves the target) |
| Wheel, or Ctrl + middle mouse | Zoom (changes the distance) |
| Numpad 1 / 3 / 7 (with Ctrl: opposite side) | Front / right / top view |
| Numpad 5 | Perspective / orthographic |
| Numpad . | Frame the selected object |
| Shift + ` | Fly mode with WASD (later) |

The camera makes the `View`: the view and projection matrices. Ray march mode uses their inverses to make
the rays, mesh mode uses them directly. Same camera, same picture.

## Lighting

Lights are objects with no shape: **Sun** (a direction) and **Point** (a position), with color and strength.

The viewport has two shading choices (a second switch in the header, later):

| Shading | Light used | Notes |
|---|---|---|
| **Solid** (default) | A fixed "studio" light that follows the view, plus the object color. Scene lights are ignored. | As in Blender's Solid mode. Always readable, cheap. |
| **Lit** | The scene lights and the world ambient light. | Shows the real look. |

Rules, the same in both modes:

- Diffuse (Lambert) + simple highlights (Blinn-Phong) at first.
- **Shadows:** ray march mode marches a shadow ray toward each light (soft shadows). Mesh mode has no
  shadows at first (shadow maps later).
- **Ambient occlusion:** cheap SDF ambient occlusion in ray march mode (later).
- If the scene has no lights, Lit mode uses one default sun, so the scene is never black.

## Picking (click to select)

1. The viewport camera turns the mouse position into a ray.
2. `sdf::raycast()` marches that one ray **on the CPU** through the SDF program. It returns the object id of
   the entry that was hit.
3. That object becomes selected. If nothing is hit, the selection is cleared.
4. Lights and cameras are picked by their distance to their icon on screen.

This works the same in both modes and without a GPU, so there is only one way picking is done.

## Showing the selection

- Ray march: the shader gets the selected ids. It draws an outline where a ray passed close to a selected
  object, or tints it.
- Mesh: the selected mesh is drawn a second time as an outline.
