cmake_minimum_required(VERSION 3.16)

project(FallingEngine)

set(CMAKE_CXX_STANDARD 20)

include(CTest)

find_package(glfw3 REQUIRED)
find_package(OpenGL REQUIRED)
find_package(glad REQUIRED)
find_package(glm REQUIRED)
find_package(imgui REQUIRED)

add_subdirectory(editor)
add_subdirectory(engine)
add_subdirectory(game)
add_subdirectory(tools)

if(BUILD_TESTING)
    add_subdirectory(engine/tests)
endif()
