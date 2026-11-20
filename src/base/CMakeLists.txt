sdfe_add_module(base
  SOURCES
    intern/log.cc
  USES
  PUBLIC_LIBS
    glm::glm
    nlohmann_json::nlohmann_json
)

sdfe_add_tests(base SOURCES tests/test_rect.cc)
