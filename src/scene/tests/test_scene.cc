#include <doctest/doctest.h>

#include "scene/document.hh"
#include "scene/sdf_program.hh"
#include "scene/shape_type.hh"

TEST_CASE("Default document")
{
  scene::register_shapes();
  scene::Document doc = scene::new_document();

  CHECK(doc.scene.objects.size() == 2);
  CHECK(doc.scene.root.objects.size() == 2);

  /* Only the sphere is an SDF shape; the light is not. */
  sdf::Program prog = scene::sdf_program(doc.scene);
  REQUIRE(prog.prims.size() == 1);
  CHECK(prog.prims[0].params[0] == 1.0f); /* default radius */
  /* The sphere is at z = 1 with radius 1, so the origin is on its surface. */
  CHECK(sdf::distance(prog, {0, 0, 0}) == doctest::Approx(0.0f).epsilon(1e-5));
}

TEST_CASE("Remove object")
{
  scene::register_shapes();
  scene::Document doc = scene::new_document();
  const scene::ObjectId id = doc.scene.selection.active;

  CHECK(scene::remove_object(doc.scene, id));
  CHECK(scene::find_object(doc.scene, id) == nullptr);
  CHECK(doc.scene.selection.active == base::NO_ID);
  CHECK_FALSE(scene::remove_object(doc.scene, id));
}
