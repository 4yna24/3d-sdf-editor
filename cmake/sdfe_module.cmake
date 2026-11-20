# Helpers that every module's CMakeLists.txt uses.
#
#   sdfe_add_module(<name>
#     SOURCES     <files>
#     USES        <modules this one may include>   (also read by tools/check_layers.py)
#     LIBS        <outside libraries, private>
#     PUBLIC_LIBS <outside libraries, also seen by users of this module>
#   )
#
# The library target is called sdfe_<name>. Headers are included as "<module>/<file>.hh".

function(sdfe_add_module name)
  cmake_parse_arguments(ARG "" "" "SOURCES;USES;LIBS;PUBLIC_LIBS" ${ARGN})
  set(target sdfe_${name})

  add_library(${target} STATIC ${ARG_SOURCES})
  target_include_directories(${target} PUBLIC ${PROJECT_SOURCE_DIR}/src)
  target_compile_features(${target} PUBLIC cxx_std_20)

  foreach(dep IN LISTS ARG_USES)
    target_link_libraries(${target} PUBLIC sdfe_${dep})
  endforeach()
  target_link_libraries(${target} PRIVATE ${ARG_LIBS})
  target_link_libraries(${target} PUBLIC ${ARG_PUBLIC_LIBS})

  sdfe_set_warnings(${target})
endfunction()

#   sdfe_add_tests(<module> SOURCES <test files>)
# Builds <module>_tests and registers it with CTest.
function(sdfe_add_tests name)
  if(NOT SDFE_BUILD_TESTS)
    return()
  endif()
  cmake_parse_arguments(ARG "" "" "SOURCES" ${ARGN})
  add_executable(${name}_tests ${ARG_SOURCES})
  target_link_libraries(${name}_tests PRIVATE sdfe_${name} sdfe_test_main)
  sdfe_set_warnings(${name}_tests)
  add_test(NAME ${name} COMMAND ${name}_tests)
endfunction()

function(sdfe_set_warnings target)
  if(MSVC)
    target_compile_options(${target} PRIVATE /W4)
  else()
    target_compile_options(${target} PRIVATE -Wall -Wextra)
  endif()
endfunction()
