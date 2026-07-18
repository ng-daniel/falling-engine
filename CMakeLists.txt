cmake_minimum_required(VERSION 3.16)

project(FallingEngine)

set(CMAKE_CXX_STANDARD 20)

find_package(glfw3 REQUIRED)
find_package(OpenGL REQUIRED)
find_package(glad REQUIRED)
find_package(glm REQUIRED)
find_package(assimp REQUIRED)

add_subdirectory(engine)
add_subdirectory(game)
add_subdirectory(tools)