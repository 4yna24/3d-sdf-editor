# Scene and editing

Where the code is: data in `scene/`, the property system in `props/`, SDF math in `sdf/`, object operators
in `editors/object/`. See [code-structure.md](code-structure.md).

## Document and scene

```
Document                     what one file holds
 ├─ Scene                    (one for now)
 ├─ file path
 └─ modified                 unsaved changes?

Scene
 ├─ objects       : map<ObjectId, Object>
 ├─ collections   : tree of Collection (name, object ids, child collections). Root = "Scene Collection".
 ├─ selection     : set of ObjectId + the active ObjectId
 └─ world         : background color, ambient light
```

```
Object
 ├─ id            a number that never changes and is never reused
 ├─ name
 ├─ visible
 ├─ transform     location (vec3), rotation (quat, shown as Euler degrees), scale (one float)
 ├─ parent        ObjectId or none (later)
 ├─ version       goes up on every change (see "Change tracking")
 └─ data          one of:  SdfShape | Light | Camera
```

- **Scale is one number** at first. Uneven scale breaks SDF distances. It can come later with a distance fix.
- **Collections are for organising**, as in Blender: show or hide a group, move objects between groups.
  They do not change any shape.
- **Lights and cameras** are objects too. They are listed in the Outliner and can be selected and moved.
  They have no SDF; the viewport draws them as line icons.
- Everything is plain data that can be copied. This keeps undo and background jobs simple.

## Shapes

```cpp
struct SdfShape {
  ShapeTypeId type;              // which shape: sphere, box, torus, ...
  std::array<float, 8> params;   // its numbers; what they mean is given by the shape type
  Material material;             // color (later: roughness, metallic)
  Combine combine;               // Union (default). Later: SmoothUnion, Subtract, Intersect + blend k
};
```

Each shape type is registered in its own small file:

```cpp
// scene/shapes/sphere.cc
void register_sphere(ShapeTypes &types)
{
  ShapeType &t = types.add("sphere", "Sphere", sdf::PRIM_SPHERE);   // id, label, formula in sdf/
  t.icon = "SPHERE";
  t.params.add_float("radius", "Radius", 1.0f).range(0.001f, 1000.0f).unit(props::Unit::Length);
}
```

- The **formula** is in `sdf/glsl/sdf_library.glsl` (math only).
- The **description** is in `scene/shapes/` (name, icon, which number is which).
- So params are a flat list of floats. That is easy to copy, save, and send to the GPU as is.
- **Combining shapes** (smooth union, subtract) is the thing SDFs do well and triangles do not.
  The `combine` field holds it from the start. The UI for it comes later.

## Property descriptions

Every type lists its properties **once**: the object (name, transform, visible), each shape type, light,
camera, world, and the preferences.

A description has: id, label, type (float, int, bool, vec3, color, enum, string), how to read and write it,
limits, step, unit, and a tooltip.

| Who uses it | For what |
|---|---|
| Properties editor | Draws a widget for each property |
| Preferences editor | The same, for preference values |
| Save / load | Writes and reads each property by its id |
| Operators | Their inputs are described the same way |
| `props.set` operator | Changes any value by its path (used by every number field) |
| Scripting (much later) | Get and set values by path |

A value is found by a **path**, for example `objects/42/shape/radius`. This is a small version of
Blender's "RNA" system.

## Changing data

- All changes go through **operators** (see [code-structure.md](code-structure.md), "Operators").
- Operators change data only with `scene` functions (`scene::add_object`, `scene::remove_object`,
  `props::set`, ...). These functions keep ids, collections and version numbers correct.
- Editors and draw engines only **read** the scene.

## Change tracking

- Each object has a `version` that goes up when it changes. The scene has one too.
- The draw code compares versions with the last ones it saw:
  - only the transform changed → update one matrix, no new mesh;
  - the shape changed → update its GPU entry, and make a new mesh in mesh mode;
  - nothing changed → do nothing.
- Operators also send a **notifier**, so the right areas redraw.

## Undo

- Undo stores a copy of the scene after each operator that has `undo = true`.
- Dragging a number or (later) moving with the mouse changes the value live, but makes **one** undo step
  when the mouse is released.
- Selection changes are not undo steps at first (Blender makes them undo steps; can be added later).

## Save / load (later)

- One JSON file, e.g. `scene.sdfe`, with `"version": 1`.
- `io/` writes the scene, using the property descriptions, so a new shape needs no save code.
- `wm` writes its own part: workspaces, area layout, each editor's `save_state()`.
- The `file.save` operator puts both parts into one file, as Blender does.
- Files from older versions are upgraded when loading.

## Export (later)

- **Image:** render the active viewport (or a scene camera) to PNG.
- **Mesh:** SDF → triangles with the same mesher as mesh mode, but finer. Saved as OBJ or STL.
