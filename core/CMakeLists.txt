#
# RT Everywhere Library
#
file(GLOB_RECURSE RT_EVERYWHERE_SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/include/*.hpp

    ${CMAKE_CURRENT_SOURCE_DIR}/source/*.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/source/*.hpp
)

add_library(RTE STATIC ${RT_EVERYWHERE_SOURCES})

target_include_directories(RTE PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include")
target_include_directories(RTE PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/source")

#
# Dependencies
#

FetchContent_Declare(glm
        GIT_REPOSITORY https://github.com/g-truc/glm.git
        GIT_TAG 2d4c4b4dd31fde06cfffad7915c2b3006402322f
)

FetchContent_Declare(luajit
        GIT_REPOSITORY https://github.com/zCubed3/LuaJIT.git
        GIT_TAG 0ec7da007289fec13edf7d57c336e74e6823b6e6
)

FetchContent_Declare(sol2
        GIT_REPOSITORY https://github.com/ThePhD/sol2.git
        GIT_TAG eba86625b707e3c8c99bbfc4624e51f42dc9e561
)

set(SOL2_BUILD_LUA FALSE BOOL)

FetchContent_MakeAvailable(glm luajit sol2)

target_include_directories(RTE PUBLIC glm LuaJIT::LuaJIT ${sol2_SOURCE_DIR}/include)
target_link_libraries(RTE PUBLIC glm LuaJIT::LuaJIT)

#
# Lua source files
#
add_custom_command(TARGET RTE PRE_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_CURRENT_SOURCE_DIR}/lua/ ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/lua
)

#
# Aliasing
#
add_library(RTE::RTE ALIAS RTE)