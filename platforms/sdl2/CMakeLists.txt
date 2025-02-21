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

if(${CMAKE_SYSTEM_NAME} MATCHES "Emscripten")
    # https://stackoverflow.com/questions/61590519/how-to-use-emscripten-ports-sdl2-and-freetype-with-cmake
    set(USE_FLAGS "-sUSE_SDL=2 -sUSE_FREETYPE=1 -sWASM=1 -fno-rtti -fno-exceptions --profiling")

    if (NOT ${CMAKE_BUILD_TYPE} MATCHES "Debug")
        set(USE_FLAGS "${USE_FLAGS} -O3 -flto=full")
    endif()

    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${USE_FLAGS}")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${USE_FLAGS}")
    set(CMAKE_EXECUTABLE_SUFFIX ".html")
endif()

add_subdirectory(dependencies/SDL EXCLUDE_FROM_ALL)

add_executable(SDL2Harness ${RT_HARNESS_SDL2_SOURCES})

target_include_directories(SDL2Harness PUBLIC SDL2 RTEverywhere)

target_link_libraries(SDL2Harness PUBLIC RTEverywhere SDL2 SDL2main)

if (SDL2_IMGUI_SUPPORT)
    target_include_directories(SDL2Harness PUBLIC dependencies/imgui)
    target_compile_definitions(SDL2Harness PUBLIC RTEVERYWHERE_IMGUI)
endif()