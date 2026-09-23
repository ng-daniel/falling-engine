add_executable(entity_handle_tests
    entity_handle_tests.cpp
)

target_link_libraries(entity_handle_tests PRIVATE engine)

add_test(NAME entity_handle_tests COMMAND entity_handle_tests)

add_executable(ecs_component_array_tests
    ecs_component_array_tests.cpp
)

target_link_libraries(ecs_component_array_tests PRIVATE engine)

add_test(NAME ecs_component_array_tests COMMAND ecs_component_array_tests)

add_executable(ecs_hierarchy_tests
    ecs_hierarchy_tests.cpp
)

target_link_libraries(ecs_hierarchy_tests PRIVATE engine)

add_test(NAME ecs_hierarchy_tests COMMAND ecs_hierarchy_tests)

add_executable(transform_tests
    transform_tests.cpp
)

target_link_libraries(transform_tests PRIVATE engine)

add_test(NAME transform_tests COMMAND transform_tests)

add_executable(material_compiler_tests
    material_compiler_tests.cpp
)

target_link_libraries(material_compiler_tests PRIVATE engine)

add_test(NAME material_compiler_tests COMMAND material_compiler_tests)
