cmake_minimum_required(VERSION 3.16)

project(ImGuiRenderExplore LANGUAGES C CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Core)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Core)

include(cmake/collect_source_code_and_resources.cmake)

add_subdirectory("tools/glfw/")
add_subdirectory("tools/pugixml/")
add_subdirectory("tools/glm/")

list(APPEND KERNEL
  "source/"
  "tools/glad/")

set(PATHS "")
list(APPEND PATHS ${KERNEL})
cmake_language(CALL COLLECT_SOURCE_CODE_AND_RESOURCES PATHS)

add_compile_definitions(IMGUI_DEFINE_MATH_OPERATORS)

add_executable(ImGuiRenderExplore

  # ImGUI
  tools/iamgui/imconfig.h
  tools/iamgui/imgui.h
  tools/iamgui/imgui_internal.h
  tools/iamgui/imgui.cpp
  tools/iamgui/imgui_demo.cpp
  tools/iamgui/imgui_draw.cpp
  tools/iamgui/imgui_tables.cpp
  tools/iamgui/imgui_widgets.cpp

  # ImGUI GLFW backend
  tools/iamgui/backends/imgui_impl_glfw.h
  tools/iamgui/backends/imgui_impl_glfw.cpp

  # ImGUI OpenGL3 backend
  tools/iamgui/backends/imgui_impl_opengl3.h
  tools/iamgui/backends/imgui_impl_opengl3.cpp

  #
  ${SOURCES}
  ${HEADERS}

  # entry point
  main.cpp
)

target_include_directories(
  ImGuiRenderExplore
  PUBLIC
  ${DIRECTORIES}
  "tools/iamgui/"
  "tools/iamgui/backends/"
  "tools/glad/include/")

target_link_libraries(ImGuiRenderExplore
  glfw
  pugixml
  glm::glm
  Qt${QT_VERSION_MAJOR}::Core)

include(GNUInstallDirs)
install(TARGETS ImGuiRenderExplore
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)
