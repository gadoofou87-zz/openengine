cmake_minimum_required(VERSION 3.5)

project(openengine LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Boost REQUIRED)
find_package(glfw3 REQUIRED)
find_package(glm REQUIRED)

include_directories(include)

add_library(openengine
    src/glad.c
    src/abstract/component.cpp
    src/abstract/disposable.cpp
    src/application.cpp
    src/camera.cpp
    src/gameobject.cpp
    src/material.cpp
    src/mesh.cpp
    src/texture.cpp)

add_executable(bsp
    src/modules/bsp/main.cpp
    src/modules/bsp/bsp.cpp)

target_include_directories(bsp PRIVATE ${Boost_INCLUDE_DIR})
target_link_libraries(openengine glfw ${CMAKE_DL_LIBS})
target_link_libraries(bsp openengine)
