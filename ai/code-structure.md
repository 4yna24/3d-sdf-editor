# Code structure

How the code is split, where each thing lives, and who calls whom.

It follows the layout of Blender's own source code (2.8 and newer), made smaller. The last table maps our
modules to Blender's, so Blender's code can be read as an example.

## Two rules

1. **Layers.** The code is split into modules, stacked in layers. A module may call modules *below* it.
   It never calls a module *above* it by name.
2. **Calling up goes through registration.** When lower code must run higher code, the higher code
   registers itself at startup (a struct of functions, or a class that implements an interface).
   The lower code only knows that interface.

Example: the window manager (`wm`) draws every area, but it does not know that an "Outliner" exists.
At startup the Outliner registers an *editor type*. When an area shows the Outliner, `wm` calls
`editor->draw_main()`, the same call it makes for every editor. So:

- the GUI does call the windows, but only through one interface;
- a new editor type is added without touching `wm`;
- each module can be built and tested without the modules above it.

The same pattern is used for shape types, operators, menus, keymaps, Properties tabs and draw engines.

## Modules

From top to bottom. "Uses" is the full list of modules it may include.

| Module | Job | Uses |
|---|---|---|
| `app` | `main()`. Calls every module's `register_*()` function, builds the default layout, starts the loop. | everything |
| `editors` | The editor types (Top bar, 3D Viewport, Outliner, Properties, Preferences) and the operators that belong to them (add object, select, orbit, save, ...). | all below |
| `wm` | Window manager: OS windows, workspaces and area layout, main loop, sending events to the right place, operator system, keymaps, menus, notifiers, undo, background jobs, `Context`. Built-in operators: undo, redo, quit, `props.set`. | ui, platform, prefs, scene, props, base |
| `draw` | Draws the 3D view: draw engines (ray march, mesh), overlays (grid, light icons, selection outline), keeps the scene on the GPU. | gpu, scene, sdf, base |
| `io` | Scene file save/load, export (OBJ, STL, PNG). | scene, sdf, props, base |
| `ui` | Widgets on top of Dear ImGui: buttons, number fields, menus, popups, tabs, theme, icons. Draws the right widget for any property from its description. | props, gpu, base |
| `scene` | The data: document, scene, objects, collections, shape types, lights, cameras, selection, and the functions that change them. Turns the scene into an SDF program. No UI, no OpenGL. | sdf, props, base |
| `prefs` | User preferences: the values and their load/save. | props, base |
| `sdf` | SDF math only: distance functions, combine operators, a flat *SDF program* (list of primitives), ray march one ray, normals, bounds, mesher. The GLSL SDF library shared with the GPU. Knows nothing about objects or UI. | base |
| `props` | The property system: describe a value once (name, type, range), then get / set / show / save it by name. | base |
| `gpu` | Thin OpenGL wrapper: shader, buffer, texture, framebuffer, mesh. | base |
| `platform` | SDL3: OS windows, OpenGL context, input events (turned into our own `Event`), file dialogs, clipboard, time. | base |
| `base` | Math (GLM), ids, `Rect`, logging, json, thread pool, small helpers. | — |

Who may touch outside libraries:

- **SDL** only in `platform`. **OpenGL** only in `gpu`.
- **ImGui** only in `ui` (its SDL backend sits in `platform`). Editors draw with `ui::` functions, not ImGui.
- So replacing SDL, OpenGL (with Vulkan) or ImGui later changes one module.

A small script, `tools/check_layers.py`, reads the `#include` lines and fails if a module includes something
not in its "Uses" list. It runs with the tests.

## Folders

```
CMakeLists.txt
extern/                    outside libraries (fetched by CMake)
tools/                     check_layers.py, small scripts
src/
  base/
  platform/
  gpu/
  props/
  sdf/
    glsl/                  sdf_library.glsl (also compiled as C++)
  prefs/
  scene/
    shapes/                one file per shape type: sphere.cc, box.cc, torus.cc, ...
  ui/
  io/
  draw/
    engines/raymarch/      engine + its shaders
    engines/mesh/
    overlay/               grid, light/camera icons, selection outline
  wm/
  editors/
    screen/                layout operators: drag edge, split, join, maximise, change editor type
    space_topbar/
    space_view3d/
    space_outliner/
    space_properties/
    space_preferences/
    object/                object operators: add, delete, duplicate, rename, select
    file/                  file operators: new, open, save, export
  app/
```

Inside one module:

```
src/scene/
  CMakeLists.txt           builds the "scene" library, links only the modules in its "Uses" list
  scene.hh  object.hh ...  public headers: other modules include only these
  intern/                  private code of this module
  tests/                   tests of this module
```

## Who owns what

```
App
 ├─ Document                the open file
 │   ├─ Scene               objects, collections, world, selection
 │   └─ file path, "modified" flag
 ├─ UndoStack
 ├─ Preferences
 ├─ draw::GpuScene          the scene on the GPU, shared by all viewports
 └─ WindowManager
     ├─ Workspaces[]        named layouts (each one is a Screen = area tree)
     └─ Windows[]           OS windows
         ├─ Top bar         global area: always there, cannot be split (later also a Status bar)
         └─ Screen          the area tree of the workspace this window shows
             └─ Areas[]
                 ├─ Editor  the editor shown now; holds its own state (viewport camera, scroll, ...)
                 └─ editors shown here before, kept so switching back keeps their state
```

- **State** (document, undo, preferences, windows) is owned by `App`. Code reaches it through `Context`.
- **Types** (shape types, editor types, operator types, menus, keymaps, Properties tabs, draw engines) are
  filled once at startup and only read after that. They live in global registries, as in Blender.
- Long-lived links use **ids** (`ObjectId`, area id), never pointers. Pointers are only held during one call.

`Context` is what every editor and operator gets. `wm` builds it for each event and each draw:

```cpp
struct Context {
  wm::WindowManager &wm;
  wm::Window *window;              // where it happens (may be null, e.g. from a script)
  wm::Area *area;
  scene::Document &doc;
  prefs::Preferences &prefs;

  scene::Scene &scene();
  scene::Object *active_object();
  void notify(const Notifier &n);  // "something changed", see below
};
```

## Windows, areas, editors

```cpp
// wm/editor.hh: what every editor type implements
class Editor {
 public:
  virtual ~Editor() = default;
  virtual void draw_header(Context &) {}                          // editor's part of the area header
  virtual void draw_main(Context &, Rect) = 0;                    // the area body
  virtual bool on_notify(const Notifier &) { return true; }       // true = "redraw me"
  virtual bool handle_event(Context &, const Event &) { return false; }  // rare; most input goes through keymaps
  virtual json save_state() const { return {}; }
  virtual void load_state(const json &) {}
};

struct EditorType {
  std::string id;       // "VIEW_3D"
  std::string label;    // "3D Viewport", shown in the editor type button
  std::string icon;
  std::string keymap;   // "View3D": keymap used while the mouse is over this editor
  std::function<std::unique_ptr<Editor>()> create;
};
```

```cpp
// wm/screen.hh: the area tree
struct Area {
  int id;
  Rect rect;                                               // set by the layout
  std::string editor_type;
  std::unique_ptr<Editor> editor;
  std::map<std::string, std::unique_ptr<Editor>> stashed;  // editors this area showed before
  bool needs_redraw = true;
};

struct ScreenNode {                    // either a split or an area
  std::unique_ptr<Area> area;          // set → this node is an area
  Direction dir;                       // otherwise: a split with two children
  float ratio;
  std::unique_ptr<ScreenNode> a, b;
};
```

`wm` draws for every area: the header (editor type button, then `editor->draw_header()`), then
`editor->draw_main()`. Resizing, splitting and the type button are written once, in `wm` and `editors/screen`.

An area has a header region and a main region. Later, in the same way, a toolbar (T) and a sidebar (N).

The top bar is also an editor (`space_topbar`) placed in a global area. It shows the registered menus
(File, Edit) and the workspace tabs. So it reuses the same code as every other area.

## Operators

Every change to the data, and every action from a menu or a hotkey, is an operator.

```cpp
// wm/operator.hh
enum class Result { Finished, Cancelled, Running, PassThrough };

struct OperatorType {
  std::string id;              // "object.delete"
  std::string label;           // "Delete": used in menus, undo history, search
  std::string description;     // tooltip
  props::StructDesc args;      // its inputs, described with the property system
  bool undo = false;           // push one undo step when it finishes

  std::function<bool(const Context &)> poll;                                // can it run now? no → greyed out
  std::function<Result(Context &, props::Values &)> exec;                   // the work, no mouse needed
  std::function<Result(Context &, props::Values &, const Event &)> invoke;  // started by mouse or key
  std::function<Result(Context &, props::Values &, const Event &)> modal;   // follows the mouse until done
};
```

- `exec` does the work. Menus, scripts and "redo" call it.
- `invoke` is used when started by a mouse click or key. It can read the mouse, then call `exec`,
  or return `Running` to become **modal**.
- `modal` gets every event while it runs: orbiting the view, dragging an area edge, and later G/R/S.
  Esc or right click returns `Cancelled` and puts the old values back.
- Operators always run through `wm`: `ctx.wm.run_operator("object.delete")`. `wm` checks `poll`, runs it,
  and pushes undo when it finishes.
- Menus and keymaps only hold operator ids. A menu item shows the operator's label, its hotkey from the
  keymap, and is greyed out when `poll` says no.

## Notifiers and versions: telling others that something changed

There are two tools, for two different questions:

| | Notifier | Version number |
|---|---|---|
| Question | "Which areas must redraw?" | "Exactly what must be updated (GPU data, meshes)?" |
| Sent by | operators: `ctx.notify({Category::Object, Action::Changed, id})` | `scene` functions, on every change |
| Read by | every editor's `on_notify()` | draw engines on `sync()`, background jobs |

Notifiers are small messages, so the Outliner can ignore a radius change. Version numbers are exact: each
object has a counter that goes up when it changes. An engine remembers the last number it saw and
compares, so it never misses a change.

## Undo

- `wm::UndoStack` stores **copies of the scene**. Scenes are small (hundreds of objects, a few hundred bytes each).
- When an operator with `undo = true` finishes, `wm` pushes a copy with the operator's label ("Delete").
- Undo puts the previous copy back and sends an "everything changed" notifier.
- Later, if scenes get big, only the changed objects are stored. Only `UndoStack` changes, the operators stay the same.

## Who calls whom

### Startup

```
main()                                         app
 ├─ platform::init()                           SDL, OpenGL context
 ├─ register_all()                             app: one line per module
 │    scene::register_shapes()                   sphere, box, torus, ...
 │    draw::register_engines()                   ray march, mesh
 │    editors::view3d::register_type()           editor type + its operators + its keymap
 │    editors::outliner::register_type()
 │    editors::object::register_operators()
 │    ...
 ├─ prefs::load()                              from the user config folder
 ├─ scene::new_document()                      default scene: a sphere and a light
 ├─ wm.open_window(default_layout())           top bar + Viewport | Outliner / Properties
 └─ wm.run()                                   main loop until Quit
```

### One pass of the main loop

```
wm.run():
  1. wait for events                  platform (sleeps while nothing happens)
  2. send each event to a handler     see "Event order" → operators change the scene
  3. collect finished jobs            e.g. a new mesh is ready → notifier
  4. give each notifier to every area editor->on_notify() → area.needs_redraw
  5. draw windows that need it        per area: header, then editor->draw_main()
```

### Event order

The first handler that uses an event stops it from going further.

```
1. running modal operator    dragging an edge, orbiting
2. open menu or popup
3. area edges                → starts "screen.area_move"
4. area under the mouse:
     a. widgets (ui)         a button or number field under the mouse
     b. editor keymap        View3D: middle mouse → "view3d.orbit"
     c. editor->handle_event()
5. window keymap             Ctrl+Z → "edit.undo", Ctrl+S → "file.save"
```

### Example: dragging "Radius" in the Properties editor

```
space_properties  draw_main(): Shape tab → for each property of the shape type: ui::property(...)
ui                the number field is dragged; it reports the new value each frame
space_properties  → ctx.wm.run_operator("props.set", {path: "objects/42/shape/radius", value})
props             writes the value into the scene
scene             object 42 version + 1
props.set         ctx.notify({Object, Changed, 42})
wm                Viewport and Properties: redraw.  Outliner: ignores it (only names matter there).
space_view3d      draw_main(): GpuScene.sync() sees object 42 is newer → updates one entry on the GPU
                  → draw engine draws → image shown in the area
mouse released    ui reports "edit finished" → wm pushes one undo step: "Change Radius"
```

### Example: clicking in the viewport to select

```
wm               mouse is over a View3D area → View3D keymap: left click → "view3d.select"
view3d.select    invoke(): ray = camera.ray_from_pixel(mouse)              space_view3d
                           hit = sdf::raycast(scene::sdf_program(scene), ray)   sdf → which object
                           scene::select(scene, hit.object, extend = shift)     scene
                           ctx.notify({Selection, Changed})
wm               Viewport, Outliner and Properties redraw
```

## Threads

- **Main thread:** events, UI, all scene changes, all OpenGL calls.
- **Worker threads** (`base::ThreadPool`): mesher, CPU fallback renderer, export.
- A job gets a **copy** of what it needs (e.g. one object's SDF program and a grid size), never the live scene.
- When a job is done, its result goes into a queue. The main loop picks it up (`wm::Jobs`) and sends a notifier.
- If the object changed while the job ran (its version is newer), the result is thrown away and a new job starts.

## Adding something new

| To add | Do this | You get for free |
|---|---|---|
| SDF shape (e.g. cone) | Formula + one `case` line in `sdf/glsl/sdf_library.glsl`. `scene/shapes/cone.cc`: label, icon, property list. | Add menu, Properties, save/load, undo, ray march, mesh, picking, export |
| Editor type | `editors/space_<name>/`: an `Editor` subclass and `register_type()`. One line in `app/register.cc`. | Listed in every area's type button; header, resizing, layout saving |
| Operator | An `OperatorType` in the module it belongs to, registered. | Can be put in any menu, keymap, button; undo |
| Menu item | One line in the menu's list (an operator id). | Label, hotkey text, greyed out by `poll` |
| Hotkey | One line in a keymap: key → operator id. | Shown in menus; editable in Preferences later |
| Preference | One field + one property line in `prefs`. | Shown in Preferences, saved |
| Viewport mode | `draw/engines/<name>/`: a `draw::Engine` subclass, registered. | Shown in the viewport mode switch |
| Properties tab | Register a tab in `space_properties`: id, icon, `poll` (when to show), `draw`. | Appears in the tab column |
| Export format | `io/export_<format>.cc` + an operator in `editors/file`. | Listed in File → Export |

## Code style

- C++20. Files are `.cc` / `.hh` with `snake_case` names, as in Blender.
- Types are `CamelCase`. Functions and variables are `snake_case`.
- Namespace = module name: `scene::`, `wm::`, `draw::`, `editors::view3d::`.
- Scene data are plain structs: copyable, no virtual functions. Virtual functions only at extension points
  (`Editor`, `draw::Engine`).
- Every module has tests in its `tests/` folder. `base`, `props`, `sdf`, `scene` and `io` are tested without a window.

## Blender name map

| Ours | Blender (`source/blender/...`) |
|---|---|
| `base` | `blenlib` (BLI) |
| `platform` | `intern/ghost` |
| `gpu` | `gpu` |
| `props` | `makesrna` (RNA) |
| `sdf` | none: this is our own topic |
| `scene` | `makesdna` (DNA) + `blenkernel` (BKE) |
| `prefs` | `UserDef` in DNA + `blenkernel` |
| `io` | `blenloader` + `io` |
| `ui` | `editors/interface` |
| `draw` | `draw` (DRW, draw engines) |
| `wm` | `windowmanager` (WM) |
| `editors` | `editors/space_*`, `editors/object`, `editors/screen` |
| `app` | `creator` |
