cmake_minimum_required(VERSION 3.21)
project(raylib_imgui_hybrid LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake")

include(Options)
include(Dependencies)
include(ImGui)

add_executable(${PROJECT_NAME} ${SOURCES})

target_include_directories(${PROJECT_NAME} PRIVATE
    src
    external/raylib-cpp/include
    external/glad/include
)

target_link_libraries(${PROJECT_NAME} PRIVATE
    raylib
    imgui
)

include(Platform)
include(Assets)