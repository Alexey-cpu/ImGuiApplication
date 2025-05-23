cmake_minimum_required(VERSION 3.16)

project(ImGuiRenderExplore LANGUAGES C CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

#find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Core)
#find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Core)

include(cmake/collect_source_code_and_resources.cmake)

add_subdirectory("tools/glfw/")

add_executable(ImGuiRenderExplore
  # headers
  tools/iamgui/imconfig.h
  tools/iamgui/imgui.h
  tools/iamgui/imgui_internal.h

  tools/glad/include/glad/glad.h
  tools/glad/include/KHR/khrplatform.h
  tools/glad/src/glad.c

  tools/iamgui/backends/imgui_impl_glfw.h
  tools/iamgui/backends/imgui_impl_opengl3.h
  tools/iamgui/backends/imgui_impl_opengl3.cpp
  tools/iamgui/backends/imgui_impl_glfw.cpp

  # sources
  tools/iamgui/imgui.cpp
  tools/iamgui/imgui_demo.cpp
  tools/iamgui/imgui_draw.cpp
  tools/iamgui/imgui_tables.cpp
  tools/iamgui/imgui_widgets.cpp
  main.cpp
)

target_include_directories(
  ImGuiRenderExplore
  PUBLIC
  "tools/iamgui"
  "tools/iamgui/backends"
  "tools/glad/include")

#target_link_libraries(ImGuiRenderExplore Qt${QT_VERSION_MAJOR}::Core)
target_link_libraries(ImGuiRenderExplore glfw)

include(GNUInstallDirs)
install(TARGETS ImGuiRenderExplore
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)
