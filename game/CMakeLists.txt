add_executable(game
    ${CMAKE_CURRENT_SOURCE_DIR}/main.cpp
)

target_include_directories(game PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(game PRIVATE
    engine
)