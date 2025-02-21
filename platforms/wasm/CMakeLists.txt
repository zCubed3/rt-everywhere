set(RT_HARNESS_WASM_SOURCES
    "wasm_main.c"
)

add_library(WASMHarness ${RT_HARNESS_WASM_SOURCES})
target_include_directories(WASMHarness PRIVATE ${RT_EVERYWHERE_DIR})