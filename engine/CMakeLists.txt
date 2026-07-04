add_library(engine

    src/core/application.cpp

    src/asset-management/asset_manager.cpp
    src/asset-management/texture_importer.cpp

    src/utils/uuid.cpp
)

target_include_directories(engine PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_link_libraries(engine PRIVATE 
  glfw 
  OpenGL::GL
  glad::glad
  glm::glm
)