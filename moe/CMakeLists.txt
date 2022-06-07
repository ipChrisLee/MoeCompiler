cmake_minimum_required(VERSION 3.18)
project(MoeCompiler)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED true)

file(GLOB_RECURSE allCFiles "src/*.cpp")
add_compile_options(-g -lm -Wall -Werror -Wextra -Wno-error=unused-parameter
        -Wno-error=deprecated-declarations
        )
add_link_options(-lpthread -v)
include_directories(src/mlib)
include_directories(src)
include_directories(src/third_party/antlr4)
add_executable(compiler ${allCFiles})
