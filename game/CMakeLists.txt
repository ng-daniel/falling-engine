add_executable(game
    ${CMAKE_CURRENT_SOURCE_DIR}/main.cpp
)

target_link_libraries(game PRIVATE
    engine
)