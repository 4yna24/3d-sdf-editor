#include <doctest/doctest.h>

#include "sdf/sdf.hh"

TEST_CASE("Sphere distance")
{
  sdf::Program prog;
  sdf::Primitive &s = prog.prims.emplace_back();
  s.type = sdf::PRIM_SPHERE;
  s.params[0] = 1.0f;

  CHECK(sdf::distance(prog, {0, 0, 0}) == doctest::Approx(-1.0f));
  CHECK(sdf::distance(prog, {3, 0, 0}) == doctest::Approx(2.0f));
}

TEST_CASE("Box distance")
{
  sdf::Program prog;
  sdf::Primitive &b = prog.prims.emplace_back();
  b.type = sdf::PRIM_BOX;
  b.params[0] = b.params[1] = b.params[2] = 2.0f; /* size 2 → faces at ±1 */

  CHECK(sdf::distance(prog, {2, 0, 0}) == doctest::Approx(1.0f));
}

TEST_CASE("Empty program is far away")
{
  CHECK(sdf::distance({}, {0, 0, 0}) > 1e9f);
}
