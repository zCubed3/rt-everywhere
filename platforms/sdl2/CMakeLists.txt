set(RT_HARNESS_SDL2_SOURCES
    "sdl2_main.c"
)

# Use our local copy of SDL
add_subdirectory(dependencies/SDL EXCLUDE_FROM_ALL)

add_library(SDL2Harness ${RT_HARNESS_SDL2_SOURCES})
target_include_directories(SDL2Harness PRIVATE ${RT_EVERYWHERE_DIR})

target_include_directories(SDL2Harness PUBLIC SDL2)
target_link_libraries(SDL2Harness PUBLIC SDL2 SDL2main)