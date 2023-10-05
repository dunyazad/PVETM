cmake_minimum_required(VERSION 3.5...3.22)

project(PVETM)

set(POLYSCOPE_BACKEND_OPENGL3_GLFW "ON" CACHE BOOL "Enable openGL3_glfw backend")
set(POLYSCOPE_BACKEND_OPENGL_MOCK "ON" CACHE BOOL "Enable openGL_mock backend")

add_subdirectory(External/polyscope)

add_executable(PVETM src/main.cpp)

target_link_libraries(PVETM polyscope)