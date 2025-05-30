cmake_minimum_required(VERSION 3.5)
project(RTEverywhere C CXX)

set(CMAKE_C_STANDARD 99)
set(CMAKE_CXX_STANDARD 11)

#
# Build folders
#

# Special folders
if (VITA)
    set(RT_EVERYWHERE_OUTPUT "Vita-${CMAKE_BUILD_TYPE}")
endif()

# Fallback folder
if (NOT DEFINED RT_EVERYWHERE_OUTPUT)
    set(RT_EVERYWHERE_OUTPUT "${CMAKE_C_COMPILER_ID}-${CMAKE_SYSTEM_PROCESSOR}-${CMAKE_BUILD_TYPE}")
endif()

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/bin/${RT_EVERYWHERE_OUTPUT}")
set(CMAKE_PDB_OUTPUT_DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/lib")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_ARCHIVE_OUTPUT_DIRECTORY})

#
# Platform variables
#
set(RT_EVERYWHERE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/core)

if (NOT DEFINED RT_EVERYWHERE_PLATFORM)
    if (NOT DEFINED RT_EVERYWHERE_PLATFORM)
        message("No native harness found for current platform! Using SDL2 harness!")
        set(RT_EVERYWHERE_PLATFORM "SDL2")
    endif()
endif()

message("RT Everywhere platform = ${RT_EVERYWHERE_PLATFORM}")

#
# Core library
#
add_subdirectory(core)

#
# Harnesses
#

if (${RT_EVERYWHERE_PLATFORM} STREQUAL SDL2)
    add_subdirectory(platforms/sdl2)
    set(RT_EVERYWHERE_HARNESS SDL2Harness)
endif()