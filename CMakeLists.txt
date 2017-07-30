cmake_minimum_required(VERSION 3.7)
project(demo)

set(CMAKE_CXX_STANDARD 11)

set(SOURCE_FILES mainA.cpp)

set(path "C:/Program Files (x86)/Microsoft Visual Studio 14.0/VC/lib")

add_executable(demo ${SOURCE_FILES})

find_package(OpenGL REQUIRED)
find_package(GLEW REQUIRED)
find_package(glfw REQUIRED)

FIND_PATH(SOIL_INCLUDE_DIRS SOIL.h PATH_SUFFIXES SOIL )
FIND_LIBRARY( SOIL_LIBRARIES SOIL)

include_directories(${OPENGL_INCLUDE_DIRS} ${GLFW_INCLUDE_DIRS} ${GLEW_INCLUDE_DIRS} "C:/Program Files (x86)/Microsoft Visual Studio 14.0/VC/include")

target_link_libraries(demo ${OPENGL_LIBRARIES} ${GLEW_LIBRARIES} ${GLFW_LIBRARIES} SOIL.lib assimp.lib freetype.lib icudt.lib icuind.lib icuiod.lib icuucd.lib icutud.lib)

