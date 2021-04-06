cmake_minimum_required(VERSION 3.8)

#
# UTF-8 Utilities
#

project(utf8_utils)

add_library(utf8_utils STATIC src/utf8.cpp src/utf8.h)
set_target_properties(
      utf8_utils PROPERTIES
      CXX_STANDARD 17
      CXX_STANDARD_REQUIRED ON
)

# Google Test
include(scripts/gtest.cmake)

# Tests executable target
add_executable(unit_tests tests/utf8_test.cpp)

target_compile_features(unit_tests PRIVATE cxx_std_17)

target_include_directories(unit_tests PUBLIC src)

# Link gtest library
target_link_libraries(unit_tests gtest_main utf8_utils)

add_test(NAME unit_tests COMMAND unit_tests)

