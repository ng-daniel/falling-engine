add_executable(ecs_hierarchy_tests
    ecs_hierarchy_tests.cpp
)

target_link_libraries(ecs_hierarchy_tests PRIVATE engine)

add_test(NAME ecs_hierarchy_tests COMMAND ecs_hierarchy_tests)
