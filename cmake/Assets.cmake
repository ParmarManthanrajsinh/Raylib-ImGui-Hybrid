add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
    "${CMAKE_SOURCE_DIR}/src/Core/Font/Roboto-Regular.ttf"
    "$<TARGET_FILE_DIR:${PROJECT_NAME}>/Roboto-Regular.ttf"
)
  