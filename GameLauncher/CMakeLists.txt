add_executable(GameLauncher src/GameApp.cpp)

target_link_libraries(GameLauncher PRIVATE Engine Game)

# Copy Assets
add_custom_command(TARGET GameLauncher POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
    "${CMAKE_SOURCE_DIR}/Engine/src/Core/Roboto-Regular.ttf"
    "$<TARGET_FILE_DIR:GameLauncher>/Roboto-Regular.ttf"
)
