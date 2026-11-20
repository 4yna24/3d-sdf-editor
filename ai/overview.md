# Overview

## Goal

A small Blender-like editor for SDF scenes:

- Add SDF shapes (sphere, box, torus, ...), move them, change their values.
- See them in a 3D viewport, drawn by **ray marching** or as a **mesh**.
- Save, load and export (later).

## Main rules

Every other part of the design follows from these.

1. **The SDF is the truth.** An object is stored as a shape and its numbers (radius, size, ...).
   Triangles are only made for drawing (mesh mode) and export. They are never saved and never edited.
2. **The scene is data.** Nothing about the scene is hard-coded in the program.
3. **Every change is an operator.** A menu item, a hotkey, a number field and a mouse drag in the viewport
   all run the same kind of small named action. That gives undo, hotkeys and menus in one place.
4. **Describe once, use everywhere.** Each type lists its properties once (name, type, range).
   The Properties editor, saving, undo and scripting (later) all read that list.
5. **Register, don't hard-code lists.** Editor types, shape types, operators, menus, render modes are added
   to registries at startup. Adding one means adding one entry.
6. **Layers.** Higher code calls lower code. Lower code calls up only through interfaces that the higher code
   registered. See [code-structure.md](code-structure.md).
7. **Draw only when something changed.** When nothing changes, the app sleeps and the GPU does no work.

## Libraries

| Need | Library | Why |
|---|---|---|
| Window, input, file dialogs | **SDL3** | One library for window, input and native file dialogs. |
| Graphics | **OpenGL 3.3 core** + **glad** | Simple, runs almost everywhere, also in software (Mesa). Can be replaced by Vulkan later, inside `gpu/`. |
| Widgets | **Dear ImGui** | Standard for tools. The area layout is our own; ImGui only draws inside the areas. |
| Math | **GLM** | Same names as GLSL, so the SDF code can be shared by CPU and GPU. |
| Save / load, preferences | **nlohmann/json** | Readable files. Every file has a `"version"` field. |
| SDF formulas | **Inigo Quilez's functions**, **hg_sdf** | Exact, tested formulas. We write the editor, not the math. |
| Image export (later) | **stb_image_write** | One header. |
| Build | **CMake** + FetchContent | One library target per module; downloads the outside libraries. |
| Tests | **doctest** | Small. |

Language: **C++20**.

## Roadmap

Each step ends with a working program.

1. CMake project with the module folders, SDL3 window, OpenGL, ImGui, `check_layers.py`.
2. `wm`: main loop, event order, area tree, dragging edges, editor type button. Top bar with File → Quit.
   Editors are still empty boxes.
3. `props`, `scene`, `sdf` basics. Operators and undo.
4. Outliner and Properties editors: add, delete, select, rename, edit values.
5. Viewport: orbit camera, ray march engine on the GPU.
6. Click to select (picking).
7. Mesh mode (mesher in background jobs).
8. Edit → Preferences.
9. Later: save/load, export, workspaces, G/R/S and gizmos, combining shapes (smooth union, subtract),
   more shapes, materials, scene lights and cameras in the UI, status bar, scripting.
