cmake_minimum_required(VERSION 3.8)

project(AdventOfCode)

# Lazy and bad practice but let's just do it this way :D

# Find all source files
file(GLOB sources "*.cpp")
foreach(source ${sources})
  # Just double check it *is* a file
  if(NOT IS_DIRECTORY ${source})
    # Get the bare file name only
    get_filename_component(name ${source} NAME_WE)
    # And setup a project
    add_executable(${name} ${source})
    # Set debug working directory for Visual Studio
    set_target_properties(${name} PROPERTIES VS_DEBUGGER_WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})
  endif()
endforeach()
