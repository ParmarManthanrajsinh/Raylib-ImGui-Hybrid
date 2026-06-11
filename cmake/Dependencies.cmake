# raylib
add_subdirectory(${CMAKE_SOURCE_DIR}/external/raylib)

# sources for your app (kept centralized)
set(SOURCES
    src/main.cpp
    src/Core/Application/Application.cpp
    src/Core/Application/Application.h
    src/Core/Application/ApplicationConfig.h
    src/Core/Application/ApplicationLayout.cpp
    src/Core/Application/ApplicationLayout.h
    src/Core/Application/ApplicationTheme.cpp
    src/Core/Application/ApplicationTheme.h
    src/Core/Application/EntryPoint.h
    src/Core/Application/EntryPoint.cpp
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
)