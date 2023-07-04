set(RT_HARNESS_SDL2_SOURCES
    "sdl2_main.cpp"
)

option(SDL2_IMGUI_SUPPORT "Enables ImGui support" ON)

if (SDL2_IMGUI_SUPPORT)
    message("SDL2 Harness has ImGui support enabled!")

    list(APPEND RT_HARNESS_SDL2_SOURCES
            "dependencies/imgui/backends/imgui_impl_sdl2.cpp"
            "dependencies/imgui/backends/imgui_impl_sdlrenderer2.cpp"

            "dependencies/imgui/imgui_demo.cpp"
            "dependencies/imgui/imgui_draw.cpp"
            "dependencies/imgui/imgui_tables.cpp"
            "dependencies/imgui/imgui_widgets.cpp"
            "dependencies/imgui/imgui.cpp"
    )
endif()

add_subdirectory(dependencies/SDL EXCLUDE_FROM_ALL)

add_library(SDL2Harness ${RT_HARNESS_SDL2_SOURCES})
target_include_directories(SDL2Harness PRIVATE ${RT_EVERYWHERE_DIR})

target_include_directories(SDL2Harness PUBLIC SDL2)
target_link_libraries(SDL2Harness PUBLIC SDL2 SDL2main)

if (SDL2_IMGUI_SUPPORT)
    target_include_directories(SDL2Harness PUBLIC dependencies/imgui)
    target_compile_definitions(SDL2Harness PUBLIC RTEVERYWHERE_IMGUI)
endif()