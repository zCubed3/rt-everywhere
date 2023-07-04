cmake_minimum_required(VERSION 3.0)
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

if (NINTENDO_3DS)
    set(RT_EVERYWHERE_OUTPUT "N3DS-${CMAKE_BUILD_TYPE}")
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
# RT Everywhere variables
#
set(RT_EVERYWHERE_SOURCES
    "core/math/real.c"
    "core/math/vectors.c"
    "core/math/matrices.c"
    "core/math/crand.c"

    "core/shapes/sphere.c"

    "core/rt_everywhere.c"
)

set(RT_EVERYWHERE_TYPE)

set(RT_EVERYWHERE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/core)

if (NOT DEFINED RT_EVERYWHERE_PLATFORM)
    if (WIN32)
        set(RT_EVERYWHERE_PLATFORM "WIN32")
    endif()

    if (APPLE)
        set(RT_EVERYWHERE_PLATFORM "APPLE")
    endif()

    if (NINTENDO_3DS)
        set(RT_EVERYWHERE_PLATFORM "NINTENDO_3DS")
    endif()

    if (NOT DEFINED RT_EVERYWHERE_PLATFORM)
        message("No native harness found for current platform! Using SDL2 harness!")
        set(RT_EVERYWHERE_PLATFORM "SDL2")
    endif()
endif()

message("RT Everywhere platform = ${RT_EVERYWHERE_PLATFORM}")

#
# Harnesses
#

if (${RT_EVERYWHERE_PLATFORM} STREQUAL SDL2)
    add_subdirectory(platforms/sdl2)
    set(RT_EVERYWHERE_HARNESS SDL2Harness)
endif()

if (${RT_EVERYWHERE_PLATFORM} STREQUAL WIN32)
    add_subdirectory(platforms/win32)
    set(RT_EVERYWHERE_HARNESS Win32Harness)
endif()

if (${RT_EVERYWHERE_PLATFORM} STREQUAL APPLE)
    add_subdirectory(platforms/macos)
    set(RT_EVERYWHERE_HARNESS macOSHarness)
endif()

if (${RT_EVERYWHERE_PLATFORM} STREQUAL NINTENDO_3DS)
    add_subdirectory(platforms/n3ds)
    set(RT_EVERYWHERE_HARNESS N3DSHarness)
endif()


add_executable(RTEverywhere ${RT_EVERYWHERE_TYPE} ${RT_EVERYWHERE_SOURCES})
target_link_libraries(RTEverywhere PUBLIC ${RT_EVERYWHERE_HARNESS})


if (${RT_EVERYWHERE_PLATFORM} STREQUAL WIN32)
    set_target_properties(RTEverywhere PROPERTIES WIN32_EXECUTABLE TRUE)
endif()

if (${RT_EVERYWHERE_PLATFORM} STREQUAL APPLE)
    target_link_libraries(RTEverywhere PUBLIC "-framework Foundation" "-framework Cocoa")
    target_compile_definitions(RTEverywhere PUBLIC "RTEVERYWHERE_FLIP_Y")
endif()

if (${RT_EVERYWHERE_PLATFORM} STREQUAL NINTENDO_3DS)
    # We need to create our SMDH
    set(SMDH_FILE "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/RTEverywhere.smdh")
    set(3DSX_FILE "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/RTEverywhere.3dsx")

    ctr_generate_smdh("${SMDH_FILE}"
        NAME "RTEverywhere"
        DESCRIPTION "RT Everywhere, the 3DS port!"
        AUTHOR "zCubed3"
    )

    ctr_create_3dsx(RTEverywhere SMDH "${SMDH_FILE}" OUTPUT "${3DSX_FILE}")
    target_compile_definitions(RTEverywhere PUBLIC "RTEVERYWHERE_FLIP_Y" "RTEVERYWHERE_SIMPLE_SCENE")
endif()

if (APPLE)
    set_target_properties(RTEverywhere PROPERTIES MACOSX_BUNDLE TRUE)
endif()