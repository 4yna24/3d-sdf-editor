# SDF Editor

An open source 3D editor where shapes are **signed distance fields (SDFs)**, not triangles.
The GUI and the code layout follow Blender (2.8 and newer).

## Build

Needs CMake 3.24+, a C++20 compiler, Python 3 with `jinja2` (for glad), and SDL3.
On Fedora: `sudo dnf install SDL3-devel python3-jinja2`.
The other libraries are downloaded by CMake.

```sh
cmake -S . -B build
cmake --build build -j
./build/src/app/sdf_editor
ctest --test-dir build        # module tests + layer check
```

## Design

The `ai/` folder is the plan for how the app behaves and how the code is built.

| File | What it covers |
|---|---|
| [ai/guide.md](ai/guide.md) | **Start here.** What is built, where to read the code, what to build next |
| [ai/overview.md](ai/overview.md) | Goal, main rules, libraries, roadmap |
| [ai/code-structure.md](ai/code-structure.md) | Modules, folders, who owns what, who calls whom, how to add new things |
| [ai/ui.md](ai/ui.md) | Window layout, areas, editors (viewport, outliner, properties), menus, preferences |
| [ai/scene.md](ai/scene.md) | Objects, shapes, properties, collections, undo, save/load |
| [ai/rendering.md](ai/rendering.md) | Ray marching, mesh mode, GPU / no GPU, viewport camera, lighting, picking |

Read `guide.md`, then `overview.md` and `code-structure.md`. The others can be read in any order.
