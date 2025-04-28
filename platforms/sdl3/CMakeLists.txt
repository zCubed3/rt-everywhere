set(RT_HARNESS_SDL3_SOURCES
    "sdl_main.cpp"
)

FetchContent_Declare(sdl
    GIT_REPOSITORY https://github.com/libsdl-org/SDL.git
    GIT_TAG release-3.2.4
)

FetchContent_MakeAvailable(sdl)

option(SDL3_IMGUI_SUPPORT "Enables ImGui support" ON)

if (SDL3_IMGUI_SUPPORT)
    message("SDL3 Harness has ImGui support enabled!")

    FetchContent_Declare(dearimgui
            GIT_REPOSITORY https://github.com/ocornut/imgui.git
            GIT_BRANCH docking
            GIT_TAG 3064e6d10cf1acd1030de24d6c213d4d1308c5fa
            GIT_SHALLOW TRUE
            GIT_PROGRESS TRUE
    )

    FetchContent_MakeAvailable(dearimgui)

    list(APPEND RT_HARNESS_SDL3_SOURCES
            "${dearimgui_SOURCE_DIR}/backends/imgui_impl_sdl3.cpp"
            "${dearimgui_SOURCE_DIR}/backends/imgui_impl_sdlrenderer3.cpp"

            "${dearimgui_SOURCE_DIR}/imgui_demo.cpp"
            "${dearimgui_SOURCE_DIR}/imgui_draw.cpp"
            "${dearimgui_SOURCE_DIR}/imgui_tables.cpp"
            "${dearimgui_SOURCE_DIR}/imgui_widgets.cpp"
            "${dearimgui_SOURCE_DIR}/imgui.cpp"
    )
endif()

add_executable(SDL3Harness ${RT_HARNESS_SDL3_SOURCES})

target_include_directories(SDL3Harness PUBLIC
        RTE::RTE
        SDL3::SDL3)

target_link_libraries(SDL3Harness PUBLIC
        RTE::RTE
        SDL3::SDL3)

if (SDL3_IMGUI_SUPPORT)
    target_include_directories(SDL3Harness PUBLIC ${dearimgui_SOURCE_DIR})
    target_compile_definitions(SDL3Harness PUBLIC RTEVERYWHERE_IMGUI)
endif()