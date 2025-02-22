#
# RT Everywhere Library
#
file(GLOB_RECURSE RT_EVERYWHERE_SOURCES
    *.cpp
    *.hpp
)

add_library(RTEverywhere ${RT_EVERYWHERE_SOURCES})

target_include_directories(RTEverywhere PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})