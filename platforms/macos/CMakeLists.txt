set(RT_HARNESS_MACOS_SOURCES
    "AppDelegate.m" "AppDelegate.h"
    "ViewController.m" "AppDelegate.h"
    "main.m"
)

add_library(macOSHarness ${RT_HARNESS_MACOS_SOURCES})
target_include_directories(macOSHarness PRIVATE ${RT_EVERYWHERE_DIR})