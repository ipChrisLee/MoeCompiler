project(Antlr4)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED true)

file(GLOB_RECURSE hppFiles "*.hpp")
file(GLOB_RECURSE cppFiles "*.cpp")

add_library(antlr4Lib STATIC ${hppFiles} ${cppFiles})
