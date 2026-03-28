set(SOURCES 
    src/Core/Application/Application.cpp
    src/Core/Application/Application.h
    src/Core/Application/ApplicationConfig.h
    src/Core/Application/EntryPoint.h
    src/Core/Base/Core.h
    src/Core/Events/ApplicationEvent.h
    src/Core/Events/Event.h
    src/Core/Events/KeyEvent.h
    src/Core/Events/MouseEvent.h
    src/Core/Input/Input.cpp
    src/Core/Input/Input.h
    src/Core/Layers/Layer.cpp
    src/Core/Layers/Layer.h
    src/Core/Layers/LayerStack.cpp
    src/Core/Layers/LayerStack.h
    src/Core/Logging/Log.cpp
    src/Core/Logging/Log.h
    src/Core/Renderer/AssetManager.cpp
    src/Core/Renderer/AssetManager.h
    src/Scene/Components.h
    src/Scene/Entity.h
    src/Scene/Scene.h
    src/Scene/Scene.cpp
)

add_library(Engine STATIC ${SOURCES})

target_include_directories(Engine PUBLIC
    src
    ../external/raylib/src
    ../external/raylib-cpp/include
    ../external/entt-3.16.0/single_include
)

target_include_directories(Engine PRIVATE
    ../external/raylib/src/external/glfw/include
)

target_link_libraries(Engine PUBLIC
    raylib
)

target_compile_definitions(Engine PRIVATE 
    $<$<CONFIG:Debug>:CORE_DEBUG>
)

# Platform-specific
if (WIN32)
    target_link_libraries(Engine PUBLIC opengl32)
elseif (UNIX AND NOT APPLE)
    target_link_libraries(Engine PUBLIC GL X11 pthread dl)
elseif (APPLE)
    target_link_libraries(Engine PUBLIC
        "-framework OpenGL"
        "-framework Cocoa"
        "-framework IOKit"
        "-framework CoreVideo"
    )
endif()
