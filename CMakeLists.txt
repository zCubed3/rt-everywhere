cmake_minimum_required(VERSION 3.0)
project(RTEverywhere C)

set(CMAKE_C_STANDARD 99)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/bin/${CMAKE_C_COMPILER_ID}-${CMAKE_SYSTEM_PROCESSOR}-${CMAKE_BUILD_TYPE}/")
set(CMAKE_PDB_OUTPUT_DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/lib")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_ARCHIVE_OUTPUT_DIRECTORY})

set(RT_EVERYWHERE_SOURCES
    "core/math/real.c"
    "core/math/vectors.c"

    "core/shapes/sphere.c"

    "core/rt_everywhere.c"
)

set(RT_EVERYWHERE_TYPE)

set(RT_EVERYWHERE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/core)

if (WIN32)
    add_subdirectory(platforms/win32)
    set(RT_EVERYWHERE_HARNESS Win32Harness)
endif()

if (APPLE)
    add_subdirectory(platforms/macos)
    set(RT_EVERYWHERE_HARNESS macOSHarness)
endif()

add_executable(RTEverywhere ${RT_EVERYWHERE_TYPE} ${RT_EVERYWHERE_SOURCES})
target_link_libraries(RTEverywhere PUBLIC ${RT_EVERYWHERE_HARNESS})

if (WIN32)
    set_target_properties(RTEverywhere PROPERTIES WIN32_EXECUTABLE TRUE)
endif()

if (APPLE)
    set_target_properties(RTEverywhere PROPERTIES MACOSX_BUNDLE TRUE)
    target_link_libraries(RTEverywhere PUBLIC "-framework Foundation" "-framework Cocoa")
    target_compile_definitions(RTEverywhere PUBLIC "RTEVERYWHERE_FLIP_Y")
endif()