# SDF Editor: Design

An open source 3D editor where shapes are **signed distance fields (SDFs)**, not triangles.
The GUI and the code layout follow Blender (2.8 and newer).

This folder is not committed. It is the plan for how the app behaves and how the code is built.

| File | What it covers |
|---|---|
| [overview.md](overview.md) | Goal, main rules, libraries, roadmap |
| [code-structure.md](code-structure.md) | Modules, folders, who owns what, who calls whom, how to add new things |
| [ui.md](ui.md) | Window layout, areas, editors (viewport, outliner, properties), menus, preferences |
| [scene.md](scene.md) | Objects, shapes, properties, collections, undo, save/load |
| [rendering.md](rendering.md) | Ray marching, mesh mode, GPU / no GPU, viewport camera, lighting, picking |

Read `overview.md`, then `code-structure.md`. The others can be read in any order.
