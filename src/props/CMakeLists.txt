# The property system: describe a value once, then show / save / set it by name.
sdfe_add_module(props
  SOURCES
    intern/props.cc
  USES
    base
)

sdfe_add_tests(props SOURCES tests/test_props.cc)
