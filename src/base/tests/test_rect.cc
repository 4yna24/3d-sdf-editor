#include <doctest/doctest.h>

#include "base/rect.hh"

using base::Rect;

TEST_CASE("Rect contains")
{
  Rect r{10, 10, 100, 50};
  CHECK(r.contains({10, 10}));
  CHECK(r.contains({109, 59}));
  CHECK_FALSE(r.contains({110, 30}));
}

TEST_CASE("Rect cut_top")
{
  Rect r{0, 0, 100, 100};
  Rect top = r.cut_top(20);
  CHECK(top.h == 20);
  CHECK(r.y == 20);
  CHECK(r.h == 80);
}
