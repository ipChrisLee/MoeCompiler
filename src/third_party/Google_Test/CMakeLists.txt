# 'Google_test' is the subproject name
project(GTest)

#   Set variable srcRoot as relative path of src
set(srcRoot ../..)

#   C++ Standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED true)

# 'lib' is the folder with Google Test sources
add_subdirectory(lib)
include_directories(${gtest_SOURCE_DIR}/include ${gtest_SOURCE_DIR})

file(GLOB_RECURSE testFiles "${srcRoot}/Tests/*.cpp")

#   Conditional Compilation
# Test codes (src/Tests) should be surrounded by `#ifdef TESTCODE` and `#endif`.
# Coding codes (which can NOT be compiled for now) should be surrounded by
# `#ifndef IGNORE_WHEN_TESTING` and `#endif`
# So that, we can test our code even if some files can't be compiled.
add_compile_definitions(TESTCODE IGNORE_WHEN_TESTING)

#   Files need to be added to compile the test target.
file(GLOB_RECURSE cppFiles "${srcRoot}/frontend/*.cpp")

# 'GTest_run' is the target name
add_executable(GTest_run ${testFiles} ${cppFiles})
target_link_libraries(GTest_run mlib)
target_link_libraries(GTest_run gtest gtest_main)
target_link_libraries(GTest_run antlr4)
