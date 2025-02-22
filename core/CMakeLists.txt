#
# RT Everywhere Library
#
file(GLOB_RECURSE RT_EVERYWHERE_SOURCES
    *.cpp
    *.hpp
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

FetchContent_MakeAvailable(glm)

target_include_directories(RTE PUBLIC glm)
target_link_libraries(RTE PUBLIC glm)

#
# Aliasing
#
add_library(RTE::RTE ALIAS RTE)