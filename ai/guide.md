# Guide: where to start

What is built, how to read the code, and what to build next.
The design is in the other files here. This file maps it to the code.

## What works now

Roadmap steps 1 and 2 are done (see [overview.md](overview.md), "Roadmap").

- The build: one CMake library per module, outside libraries downloaded, tests, layer check.
- A window with the top bar and three areas: 3D Viewport | Outliner / Properties.
- Every area has a header with the editor type button. Picking a type swaps the editor.
- Drag the edge between two areas to resize them. Esc while dragging puts it back.
- File and Edit menus. Items show their hotkeys. Items that cannot run yet are greyed out.
- File → Quit and Ctrl+Q work. File → New resets the scene (nothing shows it until the Outliner draws).
- The app sleeps when nothing happens.

Everything else is a small placeholder file with a `TODO(roadmap N)` note.
To list them all:

```sh
grep -rn "TODO(roadmap" src
```

## Build and test

See the [README](../README.md). Two checks should always pass:

- `ctest --test-dir build`: the module tests and `tools/check_layers.py`.
- `check_layers.py` fails when a module includes a module it may not use.
  The allowed list is the `USES` list in each `src/<module>/CMakeLists.txt`.

## Read the code in this order

1. `src/app/main.cc`: startup and shutdown, in order.
2. `src/app/register.cc`: every module registers its types here. One line per thing.
3. `src/app/default_layout.cc`: how a layout is built from splits and areas.
4. `src/wm/window_manager.cc`: the main loop (`run`), the event order (`handle_event`),
   drawing every area (`draw`, `draw_area`). This is the heart of the UI.
5. `src/wm/screen.hh` + `intern/screen.cc`: the area tree, layout, finding edges.
6. `src/wm/editor.hh`: what an editor implements. Then one editor:
   `src/editors/space_view3d/space_view3d.cc`.
7. `src/wm/operator.hh`, then two real operators:
   `src/editors/screen/screen_ops.cc` (a modal one) and `src/editors/file/file_ops.cc`.
8. `src/wm/menu.hh` and `src/wm/keymap.hh`: menus and hotkeys are lists of operator ids.

The lower modules (`base`, `props`, `sdf`, `scene`) are small. Read them when a step needs them.

## How the pieces connect

```
main.cc → register_all() → editor types, operators, menus, keymaps, shapes, engines go into registries
        → WindowManager.open_window() → run()

run():   events → handle_event() → modal op / menus / area edge / widgets / keymaps → operators
         notifiers → editor->on_notify()
         draw() → for each area: header (type button + editor->draw_header) + editor->draw_main
         operators clicked during draw run right after the frame
```

`wm` never names an editor. It only calls `Editor` functions and runs operators by id.

## What to build next

One roadmap step at a time. Each step ends with a working program.

### Step 3: data, operators, undo

| Do | Where |
|---|---|
| Paths (`objects/42/shape/radius`) and get/set by path | `src/props/props.hh` |
| The `props.set` operator | `src/wm/intern/wm_operators.cc` |
| Undo: store scene copies | `src/wm/intern/undo.cc` |
| `select`, `rename`, `set_transform`, collections | `src/scene/scene.hh`, `intern/scene.cc` |
| `object.add`, `object.delete`, ... | `src/editors/object/object_ops.cc` |
| Fill the Add menu with one `object.add` per shape | `src/editors/space_view3d/space_view3d.cc` |

### Step 4: Outliner and Properties

| Do | Where |
|---|---|
| `ui::property()`: the right widget for a property description | `src/ui/widgets.hh` |
| The tree, click to select, double click to rename | `src/editors/space_outliner/` |
| Registered tabs (Object, Shape, Light, Material) | `src/editors/space_properties/` |

### Step 5: viewport drawing (postponed for now)

| Do | Where |
|---|---|
| Shader, buffer, texture, framebuffer | `src/gpu/*.hh` |
| `sd_primitive()`: the formula switch, shared with the GPU | `src/sdf/glsl/sdf_library.glsl` |
| Upload the SDF program, only changed objects | `src/draw/gpu_scene.hh`, `src/scene/sdf_program.cc` |
| Ray march engine and shader | `src/draw/engines/raymarch/` |
| Grid, axes, light icons | `src/draw/overlay/` |
| Orbit camera, framebuffer per viewport, `ui::image()` | `src/editors/space_view3d/` |

### Later steps

| Step | Where |
|---|---|
| 6. Click to select | `src/sdf/raycast.hh`, a `view3d.select` operator in the View3D keymap |
| 7. Mesh mode | `src/sdf/mesher.hh`, `src/base/thread_pool.hh`, `src/wm/jobs.hh`, `src/draw/engines/mesh/` |
| 8. Preferences | `src/prefs/intern/prefs.cc` (load/save), `preferences.show` opens a second window |
| 9. Save, load, export | `src/io/`, the `file.*` operators in `src/editors/file/file_ops.cc` |

## Known shortcuts in the current code

These are fine for now. Fix them when the step that needs them comes.

- All events go to the first window (`handle_event`). A second window (Preferences) needs this fixed.
- ImGui is set up for one window only (`open_window`).
- Workspace tabs are drawn, but clicking them does nothing yet (`space_topbar.cc`).
- Editor icons are short text ("3D", "OL") until there is an icon font.
- `file.new` does not ask about unsaved changes.

## Adding things

See the table "Adding something new" in [code-structure.md](code-structure.md).
The rule is always the same: write the new thing in its own file, register it, add one line
in `src/app/register.cc` if it is a new editor or module.
