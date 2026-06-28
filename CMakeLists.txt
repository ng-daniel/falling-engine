cmake_minimum_required(VERSION 3.16)

set(PROJ_NAME "Falling-DNGINE")
project(${PROJ_NAME})

set(CMAKE_CXX_STANDARD 20)

find_package(glfw3 REQUIRED)
find_package(OpenGL REQUIRED)
find_package(glad REQUIRED)
find_package(glm REQUIRED)
find_package(assimp REQUIRED)

add_executable(${PROJ_NAME} src/main.cpp)

target_include_directories(${PROJ_NAME} PRIVATE ./include)

target_link_libraries(
  ${PROJ_NAME} PRIVATE 
  glfw 
  OpenGL::GL
  glad::glad
  glm::glm
  assimp::assimp
)

target_compile_options(${PROJ_NAME} PRIVATE -Wall -Wextra)
