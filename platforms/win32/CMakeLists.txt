set(RT_HARNESS_WIN32_SOURCES
    "winmain.c"
)

add_library(Win32Harness ${RT_HARNESS_WIN32_SOURCES})
target_include_directories(Win32Harness PRIVATE ${RT_EVERYWHERE_DIR})