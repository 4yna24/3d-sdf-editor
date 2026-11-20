#include <doctest/doctest.h>

#include "props/props.hh"

TEST_CASE("StructDesc describes properties")
{
  props::StructDesc desc;
  desc.add_float("radius", "Radius", 1.0f).range(0.001f, 1000.0f);

  const props::PropertyDesc *p = desc.find("radius");
  REQUIRE(p != nullptr);
  CHECK(p->label == "Radius");
  CHECK(p->min == doctest::Approx(0.001f));
  CHECK(std::get<float>(p->default_value) == 1.0f);
  CHECK(desc.find("missing") == nullptr);
}

TEST_CASE("Values get with fallback")
{
  props::Values v;
  v.set("type", std::string("VIEW_3D"));
  CHECK(v.get<std::string>("type") == "VIEW_3D");
  CHECK(v.get<int>("type", 7) == 7); /* wrong type → fallback */
  CHECK(v.get<float>("missing", 2.0f) == 2.0f);
}
