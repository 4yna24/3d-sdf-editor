#include <doctest/doctest.h>

#include "wm/screen.hh"

/* Editor types are not registered here, so areas have no editor. The layout does not need one. */

static wm::Screen make_screen()
{
  wm::Screen s;
  s.root = wm::make_split(wm::SplitDir::LeftRight,
                          0.5f,
                          wm::make_area("VIEW_3D"),
                          wm::make_split(wm::SplitDir::TopBottom,
                                         0.5f,
                                         wm::make_area("OUTLINER"),
                                         wm::make_area("PROPERTIES")));
  s.layout({0, 0, 200, 100}, 20);
  return s;
}

TEST_CASE("Layout fills the screen")
{
  wm::Screen s = make_screen();
  wm::Area *left = s.area_at({10, 50});
  wm::Area *top_right = s.area_at({150, 10});
  wm::Area *bottom_right = s.area_at({150, 90});

  REQUIRE(left);
  REQUIRE(top_right);
  REQUIRE(bottom_right);
  CHECK(left->editor_type == "VIEW_3D");
  CHECK(top_right->editor_type == "OUTLINER");
  CHECK(bottom_right->editor_type == "PROPERTIES");
  CHECK(left->header.h == 20);
}

TEST_CASE("Edges are found and can be moved")
{
  wm::Screen s = make_screen();
  wm::ScreenNode *split = s.split_at({100, 50});
  REQUIRE(split);
  CHECK(split->dir == wm::SplitDir::LeftRight);
  CHECK(s.split_at({50, 50}) == nullptr);

  split->ratio = wm::split_ratio_at(*split, {150, 50}, 10);
  CHECK(split->ratio == doctest::Approx(0.75f));
  /* Not past the minimum size. */
  CHECK(wm::split_ratio_at(*split, {199, 50}, 10) == doctest::Approx(0.95f));
}
