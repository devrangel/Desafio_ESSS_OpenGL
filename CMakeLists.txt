cmake_minimum_required (VERSION 3.8)

project ("Desafio_ESSS_OpenGL")

add_executable (Desafio_ESSS_OpenGL "src/main.cpp" "src/glad.c" "src/shaders/shader.cpp")

set_property (TARGET Desafio_ESSS_OpenGL PROPERTY CXX_STANDARD 14)

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET Desafio_ESSS_OpenGL PROPERTY CXX_STANDARD 20)
endif()

target_include_directories (Desafio_ESSS_OpenGL PRIVATE include)

target_link_directories (Desafio_ESSS_OpenGL PRIVATE lib)
target_link_libraries (Desafio_ESSS_OpenGL PRIVATE glfw3.lib opengl32.lib)

add_custom_command(TARGET Desafio_ESSS_OpenGL POST_BUILD
	COMMAND ${CMAKE_COMMAND} -E copy "${CMAKE_SOURCE_DIR}/src/textures/heightmap.png" "${CMAKE_BINARY_DIR}/textures/heightmap.png")