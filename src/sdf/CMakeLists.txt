# SDF math only. Knows nothing about objects or UI.
sdfe_add_module(sdf
  SOURCES
    intern/sdf.cc
  USES
    base
)

sdfe_add_tests(sdf SOURCES tests/test_sdf.cc)
