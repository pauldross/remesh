cmake_minimum_required(VERSION 3.12 FATAL_ERROR)

project(remesh)

find_package(VTK COMPONENTS 
  CommonCore
  CommonDataModel
  IOLegacy
  CommonColor
  FiltersCore
  FiltersSources
  InteractionStyle
  RenderingContextOpenGL2
  RenderingCore
  RenderingFreeType
  RenderingGL2PSOpenGL2
  RenderingOpenGL2
  RenderingLabel
)

if (NOT VTK_FOUND)
  message(FATAL_ERROR "remesh: Unable to find the VTK build folder.")
endif()

# Prevent a "command line is too long" failure in Windows.
set(CMAKE_NINJA_FORCE_RESPONSE_FILE "ON" CACHE BOOL "Force Ninja to use response files.")
add_executable(remesh MACOSX_BUNDLE Glyph3D.cxx
        remesh.h
        Point.cpp)
  target_link_libraries(remesh PRIVATE ${VTK_LIBRARIES}
)
# vtk_module_autoinit is needed
vtk_module_autoinit(
  TARGETS remesh
  MODULES ${VTK_LIBRARIES}
)