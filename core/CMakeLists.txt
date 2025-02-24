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

set(LuaJIT_STATIC On CACHE INTERNAL "")

FetchContent_MakeAvailable(glm luajit)

target_include_directories(RTE PUBLIC glm LuaJIT::LuaJIT ${sol2_SOURCE_DIR}/include)
target_link_libraries(RTE PUBLIC glm LuaJIT::LuaJIT)

#
# Lua source files
#
file(GLOB_RECURSE RTE_LUA_SRCS
    "${CMAKE_CURRENT_SOURCE_DIR}/lua/*.lua"
)

foreach(LUA_SRC ${RTE_LUA_SRCS})
    # Preserve subdirectory
    string(REPLACE "${CMAKE_CURRENT_SOURCE_DIR}/lua/" "" REL_PATH ${LUA_SRC})

    add_custom_command(
            OUTPUT "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/lua/${REL_PATH}"
            COMMENT "Copying Lua script ${REL_PATH}"
            COMMAND ${CMAKE_COMMAND} -E copy_if_different ${LUA_SRC} "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/lua/${REL_PATH}"
            DEPENDS ${LUA_SRC}
    )

    #message(NOTICE "Copying lua file ${LUA_SRC}")
    list(APPEND RTE_LUA_FILES "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/lua/${REL_PATH}")
endforeach()

add_custom_target(RTE_CopyLua ALL DEPENDS ${RTE_LUA_FILES})

add_dependencies(RTE RTE_CopyLua)

#
# Aliasing
#
add_library(RTE::RTE ALIAS RTE)