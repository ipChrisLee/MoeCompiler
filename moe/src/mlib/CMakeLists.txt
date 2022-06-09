project(MoeLib)

file(GLOB_RECURSE hppFiles "*.hpp")
file(GLOB_RECURSE cppFiles "*.cpp")

add_library(mlib STATIC ${hppFiles} ${cppFiles})