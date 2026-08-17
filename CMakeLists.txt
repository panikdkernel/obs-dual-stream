cmake_minimum_required(VERSION 3.16...3.28)
project(obs-dual-stream VERSION 1.0.0)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Find OBS
find_package(libobs REQUIRED)
find_package(obs-frontend-api REQUIRED)

# Find Qt
find_package(Qt6 COMPONENTS Core Widgets REQUIRED)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

add_library(obs-dual-stream MODULE
    src/plugin-main.cpp
    src/dual-canvas-manager.cpp
    src/dual-canvas-manager.hpp
    src/stream-output.cpp
    src/stream-output.hpp
    src/dual-stream-dock.cpp
    src/dual-stream-dock.hpp
    src/settings.cpp
    src/settings.hpp
    src/vertical-scenes-dock.cpp
    src/vertical-scenes-dock.hpp
)

target_include_directories(obs-dual-stream PRIVATE src)

target_link_libraries(obs-dual-stream
    PRIVATE
        OBS::libobs
        OBS::obs-frontend-api
        Qt6::Core
        Qt6::Widgets
)

set_target_properties(obs-dual-stream PROPERTIES PREFIX "")
