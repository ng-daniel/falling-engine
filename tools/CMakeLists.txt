add_executable(asset_header_generator
    ${CMAKE_CURRENT_SOURCE_DIR}/asset_headers/generator.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/asset_headers/main.cpp
)

target_link_libraries(asset_header_generator PRIVATE
    engine
)