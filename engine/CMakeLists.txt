add_library(engine
    
    src/assets/importers/image_importer.cpp
    src/assets/importers/texture_importer.cpp
    src/assets/importers/shader_importer.cpp
    src/assets/importers/model_importer.cpp
    src/assets/asset_manager.cpp
    src/assets/asset_warehouse_service.cpp
    src/assets/asset_metadata_service.cpp
    src/assets/asset_importer_service.cpp

    src/core/application.cpp
    src/core/window.cpp

    src/debug/logger.cpp

    src/ecs/components/transform.cpp
    src/ecs/ecs_component_registry.cpp
    src/ecs/ecs_scene_service.cpp
    src/ecs/ecs_warehouse.cpp
    src/ecs/ecs_manager.cpp

    src/serialization/asset_metadata_serializer.cpp
    src/serialization/jsonarchive.cpp
    src/serialization/serialization_helpers.cpp
    src/serialization/scene_serializer.cpp
    src/serialization/vector_serializer.cpp

    src/utils/uuid.cpp
    src/utils/random.cpp
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