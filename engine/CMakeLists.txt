add_library(engine
    src/assets/asset_manager.cpp
    src/assets/importers/image_importer.cpp
    src/assets/importers/shader_importer.cpp
    src/assets/importers/model_importer.cpp

    src/core/application.cpp

    src/serialization/asset_metadata_serializer.cpp
    src/serialization/asset_uuid_serializer.cpp
    src/serialization/jsonarchive.cpp
    src/serialization/serialization_helpers.cpp

    src/utils/uuid.cpp
)

target_include_directories(engine PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_link_libraries(engine PRIVATE 
  glfw 
  OpenGL::GL
  glad::glad
)

target_link_libraries(engine PUBLIC
  glm::glm
)