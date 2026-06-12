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

if(EMSCRIPTEN)
    set(CMAKE_EXECUTABLE_SUFFIX ".html")
endif()

# Cross-Platform Compilation Pipelines 
if(EMSCRIPTEN)
    # CMake will write this file straight to the build folder dynamically
    set(GENERATED_SHELL_PATH "${CMAKE_BINARY_DIR}/generated_shell.html")
    file(WRITE ${GENERATED_SHELL_PATH}
      "<!DOCTYPE html>
      <html lang=\"en\">
      <head>
      <meta charset=\"UTF-8\">
      <title>Modular Raylib + ImGui App</title>
      <style>
          html, body { margin: 0; padding: 0; width: 100%; height: 100%; background-color: #121212; overflow: hidden; display: flex; justify-content: center; align-items: center; }
          canvas { width: 100vw !important; height: 100vh !important; display: block; border: none; outline: none; }
      </style>
      </head>
      <body>
      <canvas id=\"canvas\" oncontextmenu=\"event.preventDefault()\"></canvas>
      <script>
          var Module = {
              canvas: (function() { return document.getElementById('canvas'); })(),
              arguments: [\"--emscripten-webgl-context-attributes\", \"{ \\\"alpha\\\": false, \\\"antialias\\\": true }\"]
          };
      </script>
      {{{ SCRIPT }}}
      </body>
      </html>"
    )

    # Web Assembly Linker Target Bindings
    target_link_libraries(${PROJECT_NAME} PRIVATE raylib imgui)
    target_link_options(${PROJECT_NAME} PRIVATE 
        "-sUSE_GLFW=3"
        "-sFULL_ES2=1"
        "--shell-file=${GENERATED_SHELL_PATH}"
        
        # 🚀 AUTOMATED ASSET PACKING: Mounts your entire 'src' folder tree to the browser's virtual root (/)
        # This instantly bundles your fonts and any present or future image asset folders!
        "--preload-file=${CMAKE_SOURCE_DIR}/src@/"
    )
else()
    # Native Desktop Compilation Targets
    target_link_libraries(${PROJECT_NAME} PRIVATE raylib imgui)
    include(Platform)
    include(Assets)
endif()
