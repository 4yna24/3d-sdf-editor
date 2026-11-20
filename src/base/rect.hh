#pragma once

#include "base/math.hh"

namespace base {

/** A rectangle in window units. (0, 0) is the top left corner. */
struct Rect {
  float x = 0.0f, y = 0.0f, w = 0.0f, h = 0.0f;

  bool contains(vec2 p) const
  {
    return p.x >= x && p.x < x + w && p.y >= y && p.y < y + h;
  }

  /** The same rectangle, `d` smaller on every side. */
  Rect inset(float d) const
  {
    return {x + d, y + d, glm::max(w - 2.0f * d, 0.0f), glm::max(h - 2.0f * d, 0.0f)};
  }

  /** Cut `amount` off the top and return it. This rectangle keeps the rest. */
  Rect cut_top(float amount)
  {
    amount = glm::min(amount, h);
    Rect top{x, y, w, amount};
    y += amount;
    h -= amount;
    return top;
  }
};

}  // namespace base
