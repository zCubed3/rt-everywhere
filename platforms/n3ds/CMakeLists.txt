set(RT_HARNESS_N3DS_SOURCES "n3ds_main.c")

add_library(N3DSHarness ${RT_HARNESS_N3DS_SOURCES})
target_include_directories(N3DSHarness PRIVATE ${RT_EVERYWHERE_DIR})