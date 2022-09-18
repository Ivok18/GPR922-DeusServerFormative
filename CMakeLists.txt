# CMakeList.txt : Top-level CMake project file, do global configuration
# and include sub-projects here.
#
cmake_minimum_required (VERSION 3.8)

# Name of the solution
project ("SAE-GPR5100")

# Include sub-projects.
include_directories(Common/include/)
file(GLOB_RECURSE PROJECT_COMMON common/src/*.cpp common/include/*.h)

set(EXECUTABLE_CUSTOM_NAME "DeusMulti-Client")
add_subdirectory ("DeusMulti-Client")

set(EXECUTABLE_CUSTOM_NAME "DeusMulti-Server")
add_subdirectory ("DeusMulti-Server")












