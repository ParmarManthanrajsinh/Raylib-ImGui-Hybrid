set(SOURCES
    src/EditorApp.cpp
    src/ApplicationLayout.cpp
    src/ApplicationLayout.h
    src/ApplicationTheme.cpp
    src/ApplicationTheme.h
    src/EditorLayer.cpp
    src/EditorLayer.h
)

add_executable(Editor ${SOURCES})

target_include_directories(Editor PRIVATE
    ../external/imgui
    ../external/imgui/backends
    ../external/raylib/src/external/glfw/include
    src
)

target_link_libraries(Editor PRIVATE Engine Game imgui)

# Copy Assets
add_custom_command(TARGET Editor POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
    "${CMAKE_SOURCE_DIR}/Engine/src/Core/Roboto-Regular.ttf"
    "$<TARGET_FILE_DIR:Editor>/Roboto-Regular.ttf"
)
