#pragma once

/* The area layout: a tree of splits whose leaves are areas. See ai/ui.md, "Areas". */

#include <functional>
#include <map>
#include <memory>
#include <string>

#include "base/rect.hh"
#include "wm/editor.hh"

namespace wm {

/** Gap between areas, and how close to an edge the mouse must be to drag it. */
inline constexpr float AREA_GAP = 1.0f;
inline constexpr float AREA_EDGE_GRAB = 4.0f;

struct Area {
  int id = 0;
  Rect rect;        /* Set by the layout. */
  Rect header;      /* Top line of `rect`. */
  Rect main;        /* The rest. */
  bool global = false; /* Top bar: always there, no type button, cannot be split. */
  bool needs_redraw = true;

  std::string editor_type;
  std::unique_ptr<Editor> editor;
  std::map<std::string, std::unique_ptr<Editor>> stashed; /* Editors this area showed before. */

  /** Show another editor type here. The old editor is kept, so switching back keeps its state. */
  void set_editor_type(const std::string &type);
};

enum class SplitDir {
  LeftRight, /* Children side by side; the edge between them is vertical. */
  TopBottom, /* Children stacked; the edge is horizontal. */
};

/** Either an area (leaf) or a split with two children. */
struct ScreenNode {
  int id = 0;
  std::unique_ptr<Area> area;

  SplitDir dir = SplitDir::LeftRight;
  float ratio = 0.5f; /* Share of the first child. */
  std::unique_ptr<ScreenNode> a, b;

  Rect rect; /* Set by the layout. */

  bool is_area() const
  {
    return area != nullptr;
  }
};

struct Screen {
  std::unique_ptr<ScreenNode> root;

  /** Give every node and area its rectangle inside `rect`. */
  void layout(const Rect &rect, float header_height);

  Area *area_at(base::vec2 p);
  /** The split whose edge is under `p`, or null. */
  ScreenNode *split_at(base::vec2 p);
  ScreenNode *find_node(int id);
  void for_each_area(const std::function<void(Area &)> &fn);
};

/** A named layout, shown as a tab in the top bar. */
struct Workspace {
  std::string name;
  Screen screen;
};

/* Helpers to build layouts. */
std::unique_ptr<Area> new_area(const std::string &editor_type);
std::unique_ptr<ScreenNode> make_area(const std::string &editor_type);
std::unique_ptr<ScreenNode> make_split(SplitDir dir,
                                       float ratio,
                                       std::unique_ptr<ScreenNode> a,
                                       std::unique_ptr<ScreenNode> b);

/** The ratio that puts the edge of `split` at `p`, keeping both sides at least `min_size`. */
float split_ratio_at(const ScreenNode &split, base::vec2 p, float min_size);

}  // namespace wm
