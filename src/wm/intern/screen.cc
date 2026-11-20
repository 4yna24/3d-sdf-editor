#include "wm/screen.hh"

#include <algorithm>
#include <cmath>

namespace wm {

static int g_next_id = 1;

/* ---- Area ---- */

void Area::set_editor_type(const std::string &type)
{
  if (type == editor_type && editor) {
    return;
  }
  if (editor) {
    stashed[editor_type] = std::move(editor);
  }
  editor_type = type;
  needs_redraw = true;

  auto it = stashed.find(type);
  if (it != stashed.end()) {
    editor = std::move(it->second);
    stashed.erase(it);
    return;
  }
  const EditorType *et = find_editor_type(type);
  editor = (et && et->create) ? et->create() : nullptr;
}

std::unique_ptr<Area> new_area(const std::string &editor_type)
{
  auto area = std::make_unique<Area>();
  area->id = g_next_id++;
  area->set_editor_type(editor_type);
  return area;
}

std::unique_ptr<ScreenNode> make_area(const std::string &editor_type)
{
  auto node = std::make_unique<ScreenNode>();
  node->id = g_next_id++;
  node->area = new_area(editor_type);
  return node;
}

std::unique_ptr<ScreenNode> make_split(SplitDir dir,
                                       float ratio,
                                       std::unique_ptr<ScreenNode> a,
                                       std::unique_ptr<ScreenNode> b)
{
  auto node = std::make_unique<ScreenNode>();
  node->id = g_next_id++;
  node->dir = dir;
  node->ratio = ratio;
  node->a = std::move(a);
  node->b = std::move(b);
  return node;
}

/* ---- Layout ---- */

static void layout_node(ScreenNode &node, const Rect &rect, float header_height)
{
  node.rect = rect;
  if (node.is_area()) {
    Area &area = *node.area;
    area.rect = rect.inset(AREA_GAP);
    Rect rest = area.rect;
    area.header = rest.cut_top(header_height);
    area.main = rest;
    return;
  }
  Rect ra = rect, rb = rect;
  if (node.dir == SplitDir::LeftRight) {
    ra.w = std::round(rect.w * node.ratio);
    rb.x = rect.x + ra.w;
    rb.w = rect.w - ra.w;
  }
  else {
    ra.h = std::round(rect.h * node.ratio);
    rb.y = rect.y + ra.h;
    rb.h = rect.h - ra.h;
  }
  layout_node(*node.a, ra, header_height);
  layout_node(*node.b, rb, header_height);
}

void Screen::layout(const Rect &rect, float header_height)
{
  if (root) {
    layout_node(*root, rect, header_height);
  }
}

/* ---- Lookups ---- */

static void for_each_node(ScreenNode *node, const std::function<bool(ScreenNode &)> &fn)
{
  if (!node || !fn(*node)) {
    return;
  }
  if (!node->is_area()) {
    for_each_node(node->a.get(), fn);
    for_each_node(node->b.get(), fn);
  }
}

void Screen::for_each_area(const std::function<void(Area &)> &fn)
{
  for_each_node(root.get(), [&](ScreenNode &n) {
    if (n.is_area()) {
      fn(*n.area);
    }
    return true;
  });
}

Area *Screen::area_at(base::vec2 p)
{
  Area *found = nullptr;
  for_each_area([&](Area &a) {
    if (a.rect.contains(p)) {
      found = &a;
    }
  });
  return found;
}

ScreenNode *Screen::split_at(base::vec2 p)
{
  ScreenNode *found = nullptr;
  for_each_node(root.get(), [&](ScreenNode &n) {
    if (found || n.is_area() || !n.rect.contains(p)) {
      return false;
    }
    const Rect &r = n.rect;
    const float edge = n.dir == SplitDir::LeftRight ? r.x + std::round(r.w * n.ratio)
                                                    : r.y + std::round(r.h * n.ratio);
    const float pos = n.dir == SplitDir::LeftRight ? p.x : p.y;
    if (std::abs(pos - edge) <= AREA_EDGE_GRAB) {
      found = &n;
      return false;
    }
    return true; /* Look in the children. */
  });
  return found;
}

ScreenNode *Screen::find_node(int id)
{
  ScreenNode *found = nullptr;
  for_each_node(root.get(), [&](ScreenNode &n) {
    if (n.id == id) {
      found = &n;
    }
    return !found;
  });
  return found;
}

float split_ratio_at(const ScreenNode &split, base::vec2 p, float min_size)
{
  const Rect &r = split.rect;
  const bool lr = split.dir == SplitDir::LeftRight;
  const float size = lr ? r.w : r.h;
  if (size <= 2.0f * min_size) {
    return 0.5f;
  }
  const float pos = lr ? p.x - r.x : p.y - r.y;
  return std::clamp(pos, min_size, size - min_size) / size;
}

}  // namespace wm
