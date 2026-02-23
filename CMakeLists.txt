cmake_minimum_required(VERSION 3.10)
project(HotUtils)

option(HOT_UTILS_BUILD_TESTS "Build HotUtils tests" ON)

add_library(hot_utils INTERFACE)
add_library(hot_utils::hot_utils ALIAS hot_utils)

target_include_directories(hot_utils INTERFACE
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
)

if(HOT_UTILS_BUILD_TESTS)
  find_package(GTest REQUIRED)

  enable_testing()

  file(GLOB TEST_SOURCES CONFIGURE_DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/tests/*.cpp)
  add_executable(hot_utils_tests ${TEST_SOURCES})

  target_link_libraries(hot_utils_tests PRIVATE hot_utils GTest::gtest_main)

  include(GoogleTest)
  gtest_discover_tests(hot_utils_tests)
endif()
