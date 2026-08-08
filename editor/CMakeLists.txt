add_executable(editor
    ${CMAKE_CURRENT_SOURCE_DIR}/main.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/core/editor.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/windows/asset_browser.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/windows/inspector.cpp
)

target_include_directories(editor PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_link_libraries(editor PRIVATE
    engine
    imgui::imgui
)