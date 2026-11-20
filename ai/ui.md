# User interface

The layout follows Blender 2.8 and newer. The names are Blender's too, so Blender's manual helps.
How it is coded: [code-structure.md](code-structure.md), "Windows, areas, editors".

```
┌──────────────────────────────────────────────────────────────────────┐
│ File  Edit │ Layout │                                     (top bar)   │
├──────────────────────────────────────────────┬───────────────────────┤
│ [▼] View  Add               [Ray march|Mesh] │ [▼]                   │
│                                              │ ▾ Scene Collection    │
│                                              │   ▾ Collection        │
│                3D view                       │       Sphere          │
│                                              │       Light           │
│                                              ├───────────────────────┤
│                                              │ [▼]                   │
│                                              │ [O] Sphere            │
│                                              │ [S] ▾ Transform       │
│                                              │ [M]   Location 0 0 1  │
│                                              │     ▾ Sphere          │
│                                              │       Radius  1.0     │
└──────────────────────────────────────────────┴───────────────────────┘
```

## Words

| Word | Meaning |
|---|---|
| **Window** | An OS window. There is one at the start; Preferences opens a second one. |
| **Top bar** | The line at the top: menus (File, Edit), then workspace tabs. A *global area*: it cannot be split. |
| **Workspace** | A named layout, shown as a tab in the top bar ("Layout", later "Modeling", ...). |
| **Screen** | The layout of one workspace: everything below the top bar, filled by areas. |
| **Area** | One rectangle in the screen. Areas never overlap and together always fill the screen. |
| **Region** | A part of an area: the header and the main part. Later also a toolbar (T) and a sidebar (N). |
| **Editor** | What an area shows: 3D Viewport, Outliner, Properties, ... Any area can show any editor. |

## Areas

The screen is a **tree of splits**:

```
Split (left | right, 70%)
 ├─ Area: 3D Viewport
 └─ Split (top / bottom, 40%)
     ├─ Area: Outliner
     └─ Area: Properties
```

- Each split has a direction and a ratio. Each leaf is an area.
- **Dragging an edge** changes the ratio. Areas always stay "maximised": no gaps, no overlap.
- **Split / join** (later): right click on an edge, as in Blender.
- **Maximise** (later): Ctrl+Space makes one area fill the screen until pressed again.
- **Floating** (later): an area can be moved into its own OS window. The window holds a one-area screen.

The layout is our own code. ImGui only draws the widgets inside each area. ImGui's docking is not used:
it works with tabs and floating panels, not Blender-style tiles.

### Every area works the same way

1. A **header** at the top. On its left: the **editor type button** (`[▼]`, the icon of the current editor).
   It lists every registered editor type. Picking one replaces the editor in this area.
2. The rest of the header belongs to the editor (menus, the viewport mode switch on the right).
3. The **main region** below, drawn by the editor.

The area does the header, the type button, resizing and passing input on. The editor only fills in its
parts, so a new editor type gets all of this for free. Each area keeps its own editor state: two viewports
can have different cameras and modes. Switching an area to another type and back keeps the old state.

## Editors in the first version

### 3D Viewport

- Shows the scene from this viewport's camera (see [rendering.md](rendering.md)).
- Header: `View` and `Add` menus on the left. **Mode switch** `Ray march | Mesh` on the right.
  Each viewport has its own mode.
- Shift+A opens the same Add menu at the mouse.
- Left click selects the object under the mouse. Shift+click adds it to the selection.
  Clicking empty space clears the selection.
- Camera: middle mouse orbits, Shift+middle mouse pans, the wheel zooms, numpad keys give fixed views,
  numpad `.` frames the selection.
- Draws a ground grid, the axes, light and camera icons, and an outline on selected objects.
- **Later:** G/R/S to grab, rotate and scale with the mouse, and gizmo arrows. Moving with the mouse
  is the same as changing the Location number. It is one modal operator that becomes one undo step
  when the mouse is released.

### Outliner

- A tree: "Scene Collection", the collections in it, and their objects.
- Click selects (the same selection as the viewport). Double click renames. The eye icon hides.
- Right-click menu: Delete, Duplicate, New Collection.
- Dragging an object onto a collection moves it there (later).

### Properties

- Shows the **active object** (the one selected last).
- A **tab column** on the left, as in Blender. First tabs:

| Tab | Shows | Shown when |
|---|---|---|
| Object | name, location, rotation, scale, visible | an object is active |
| Shape | the shape's values (radius, size, ...) | the active object is an SDF shape |
| Light | type, color, strength | the active object is a light |
| Material | color | the active object is an SDF shape |
| World (later) | background, ambient light | always |

- Tabs are registered (id, icon, "shown when" check, draw function). A new tab is a new entry.
- The contents are drawn **automatically** from the property descriptions (see [scene.md](scene.md)).
  A float becomes a number field, a vec3 becomes three, a color a color picker, an enum a dropdown.
- Drag a number with the mouse, or double click and type. Ctrl+Z works.

### Top bar

- Menus on the left. Every item is an operator id. A menu shows the operator's label and hotkey,
  and greys the item out when it cannot run.

| Menu | Items in the first version |
|---|---|
| **File** | New, Open..., Save, Save As..., Export ▸, Quit (Open/Save/Export disabled until they exist) |
| **Edit** | Undo, Redo, Preferences... |

- Workspace tabs right of the menus. At first there is one fixed workspace, "Layout".
  Switching a tab swaps the screen (area tree) of the window.

## Preferences

**Edit → Preferences** opens a separate OS window that shows the Preferences editor, as in Blender.

- A section list on the left, values on the right. It is drawn from property descriptions, like Properties.
- Saved in the user config folder (`~/.config/sdf-editor/preferences.json` on Linux), not in the scene file.
- Sections, to be filled later: Interface (UI scale, theme), Viewport (mesh resolution, ray march quality),
  Input (orbit style, keymap), System (GPU or CPU renderer).

## Input

- Each event goes first to widgets, then to the keymap of the editor under the mouse, then to the window
  keymap (Ctrl+Z, Ctrl+S). The exact order is in [code-structure.md](code-structure.md), "Event order".
- Hotkeys are tables of `key → operator id`. Later, Preferences → Input edits these tables.
