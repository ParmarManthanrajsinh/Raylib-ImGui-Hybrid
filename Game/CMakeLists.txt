set(SOURCES 
    src/GameLayer.cpp
    src/GameLayer.h
)

add_library(Game STATIC ${SOURCES})

target_include_directories(Game PUBLIC
    src
    ../Engine/src
    ../external/raylib/src
    ../external/raylib-cpp/include
)

target_link_libraries(Game PUBLIC
    Engine
)
