add_library(engine

    src/core/application.cpp

    src/assets/asset_manager.cpp
    src/assets/texture_importer.cpp

    src/serialization/asset_metadata_serializer.cpp

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