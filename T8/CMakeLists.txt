# Copyright (c)
# 2021 Sharjeel Ali <sharjeelali@cmail.carleton.ca>
# 2018 David Mould <mould@scs.carleton.ca>

cmake_minimum_required(VERSION 3.10)

project(Tutorial8 LANGUAGES CXX)

# Use a modern C++ standard
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Set the path to the external libraries folder
set(LIBRARY_PATH "C:/Libraries" CACHE PATH "Path to external libraries")

# Source files
set(SRCS
    ${CMAKE_CURRENT_SOURCE_DIR}/main.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/camera.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/fetchInput.cpp
)

# Header files (optional in CMake, but useful for IDE organization)
set(HDRS
    ${CMAKE_CURRENT_SOURCE_DIR}/include/camera.h
    ${CMAKE_CURRENT_SOURCE_DIR}/include/fetchInput.h
)

# Shader/resource files (optional in build, but useful for IDE organization)
set(RESOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/resource/shaderPhong.vert
    ${CMAKE_CURRENT_SOURCE_DIR}/resource/shaderPhong.frag
    ${CMAKE_CURRENT_SOURCE_DIR}/resource/waterShader.vert
    ${CMAKE_CURRENT_SOURCE_DIR}/resource/waterShader.frag
    ${CMAKE_CURRENT_SOURCE_DIR}/resource/skybox.vert
    ${CMAKE_CURRENT_SOURCE_DIR}/resource/skybox.frag
    ${CMAKE_CURRENT_SOURCE_DIR}/resource/shadow.frag
    ${CMAKE_CURRENT_SOURCE_DIR}/resource/shadow.vert

)

add_executable(${PROJECT_NAME}
    ${SRCS}
    ${HDRS}
    ${RESOURCES}
)

# Include directories
target_include_directories(${PROJECT_NAME} PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${CMAKE_CURRENT_SOURCE_DIR}/resource
    ${CMAKE_CURRENT_SOURCE_DIR}/assets
    ${LIBRARY_PATH}/include
)

# OpenGL
find_package(OpenGL REQUIRED)

# Platform-specific libraries
if(WIN32)
    find_library(GLEW_LIBRARY NAMES glew32s HINTS ${LIBRARY_PATH}/lib REQUIRED)
    find_library(GLFW_LIBRARY NAMES glfw3 HINTS ${LIBRARY_PATH}/lib REQUIRED)
    find_library(SOIL_LIBRARY NAMES SOIL HINTS ${LIBRARY_PATH}/lib REQUIRED)

    # Often needed when using static GLEW on Windows
    target_compile_definitions(${PROJECT_NAME} PRIVATE GLEW_STATIC)

else()
    find_library(GLEW_LIBRARY NAMES GLEW REQUIRED)
    find_library(GLFW_LIBRARY NAMES glfw REQUIRED)
    find_library(SOIL_LIBRARY NAMES SOIL REQUIRED)
endif()

target_link_libraries(${PROJECT_NAME} PRIVATE
    OpenGL::GL
    ${GLEW_LIBRARY}
    ${GLFW_LIBRARY}
    ${SOIL_LIBRARY}
)

# Visual Studio debug postfix
if(WIN32)
    set_target_properties(${PROJECT_NAME} PROPERTIES DEBUG_POSTFIX "_d")
endif()