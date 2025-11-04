cmake_minimum_required(VERSION 3.14)
project(my_project)

# GoogleTest requires at least C++17
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(FetchContent)
FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/03597a01ee50ed33e9dfd640b249b4be3799d395.zip
)
# For Windows: Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

enable_testing()

# Collect all .cc files in the unit_tests directory
file(GLOB UNIT_TEST_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/unit_tests/*.cc")

# Add the executable
add_executable(unit_tests ${UNIT_TEST_SOURCES})

# Add the prj folder to the include paths
target_include_directories(unit_tests PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/../prj")

# Link the GoogleTest library
target_link_libraries(unit_tests GTest::gtest_main)

# Discover and register tests
include(GoogleTest)
gtest_discover_tests(unit_tests)