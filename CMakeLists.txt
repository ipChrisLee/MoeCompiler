cmake_minimum_required(VERSION 3.18)
#   [change compiler to clang++](https://www.jetbrains.com/help/clion/how-to-switch-compilers-in-clion.html#prj-compilers)
project(MoeCompiler)

#   C++ Standards
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED true)
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -D_GLIBCXX_DEBUG")

#   Files of project (exclude libraries)
file(GLOB_RECURSE cppFiles
		"src/main.cpp"
		"src/support/*.cpp"
		"src/IR/*.cpp"
		"src/frontend/*.cpp"
		)

#   Include Paths
# From [CMake Tutorial](https://cmake.org/cmake/help/latest/command/include_directories.html?highlight=include_directories):
# The include directories are added to the INCLUDE_DIRECTORIES directory property for the current CMakeLists file.
# They are also added to the INCLUDE_DIRECTORIES target property for each target in the current CMakeLists file.
include_directories(src)
include_directories(src/mlib)
include_directories(src/third_party/antlr4-runtime)

#   Compile and Options
# From
#   [1](https://cmake.org/cmake/help/latest/command/add_compile_options.html?highlight=add_compile_options)
#   [2](https://cmake.org/cmake/help/latest/command/add_link_options.html?highlight=add_link_options)
# These options will add to subdirectories.
add_compile_options(
		-g -Wall -Werror -Wextra -pthread
		-Wno-unused-parameter
		-Wno-unused-variable
		-Wno-unknown-pragmas
)
add_link_options(-lpthread -lm -v -pthread)

#   Library Subdirectories
add_subdirectory(src/mlib)                      #   moe lib
add_subdirectory(src/third_party/Google_Test)   #   Google-Test
add_subdirectory(src/third_party/antlr4-runtime)

#   Target of compiler
add_executable(compiler ${cppFiles})
target_link_libraries(compiler PUBLIC mlib)
target_link_libraries(compiler PUBLIC antlr4)

# TODO: Add -O2 release version compiler.
