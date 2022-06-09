# 'Google_test' is the subproject name
project(Google_tests)

# 'lib' is the folder with Google Test sources
add_subdirectory(lib)
add_compile_definitions(TESTING)    #   conditional compilation
include_directories(${gtest_SOURCE_DIR}/include ${gtest_SOURCE_DIR})
include_directories(../..)  #   test files should include from `moe/src/`
file(GLOB_RECURSE allTestFiles "../../Tests/*.cpp")

# 'Google_Tests_run' is the target name
add_executable(Google_Tests_run ${allTestFiles})
target_link_libraries(Google_Tests_run gtest gtest_main)
