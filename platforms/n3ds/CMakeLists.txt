if (${RT_EVERYWHERE_PLATFORM} NOT STREQUAL NINTENDO_3DS)
    message(FATAL_ERROR "Tried using N3DS harness on a non N3DS platform!")
endif()

set(RT_HARNESS_N3DS_SOURCES "n3ds_main.c")

add_executable(N3DSHarness ${RT_HARNESS_N3DS_SOURCES})
target_include_directories(N3DSHarness PRIVATE ${RT_EVERYWHERE_DIR})

# We need to create our SMDH
set(SMDH_FILE "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/RTEverywhere.smdh")
set(3DSX_FILE "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/RTEverywhere.3dsx")

ctr_generate_smdh("${SMDH_FILE}"
    NAME "RTEverywhere"
    DESCRIPTION "RT Everywhere, the 3DS port!"
    AUTHOR "zCubed3"
)

ctr_create_3dsx(RTEverywhere SMDH "${SMDH_FILE}" OUTPUT "${3DSX_FILE}")
target_compile_definitions(RTEverywhere PUBLIC "RTE_FLIP_Y" "RTE_SIMPLE_SCENE")