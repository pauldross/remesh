cmake_minimum_required(VERSION 3.12 FATAL_ERROR)

project(remesh)

# Prevent a "command line is too long" failure in Windows.
set(CMAKE_NINJA_FORCE_RESPONSE_FILE "ON" CACHE BOOL "Force Ninja to use response files.")
add_executable(remesh MACOSX_BUNDLE remesh.cpp
        remesh.h
        Point.cpp
        VTKParser/depend/VTKParser.h
        VTKParser/depend/VTKParser.cpp
        argparse.hpp
)

# Include the directories with header files
target_include_directories(remesh PRIVATE depend VTKParser/depend)
