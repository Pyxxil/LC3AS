cmake_minimum_required(VERSION 3.6)

project(LC3_ASSEMBLER)

set(CMAKE_CXX_STANDARD 17)

if ("${CMAKE_BUILD_TYPE}" STREQUAL "Debug")
    set(CMAKE_VERBOSE_MAKEFILE ON)
endif ()

option(Debug OFF)

if (Debug)
    # To get around a problem with CLion where defining CMAKE_BUILD_TYPE as Debug
    # in Preferences for ADDONS_Debug would cause it to think it's the same as a
    # a normal debug build
    set(CMAKE_BUILD_TYPE "Debug")
endif ()

option(ALLOW_EXTRAS OFF)

### Populated by Lib/CMakeLists.txt
set(LIBRARY_SOURCE_FILES "")
set(LIBRARY_HEADER_FILES "")
set(LIBRARY_INCLUDE_DIRECTORIES "")

### Populated by Assembler/CMakeLists.txt
set(ASSEMBLER_SOURCE_FILES "")
set(ASSEMBLER_HEADER_FILES "")
set(ASSEMBLER_INCLUDE_DIRECTORIES "")

set(DEBUG_FLAGS ${COMPILE_FLAGS} -O0 -g -pg -pedantic)

if (NOT WIN32)
    set(DEBUG_FLAGS ${DEBUG_FLAGS} -Wall -Werror -Wextra -Wshadow -Wnon-virtual-dtor -Wconversion -Wno-error=conversion)
    set(DEBUG_FLAGS ${DEBUG_FLAGS} -Wold-style-cast -Wcast-align -Wunused -Woverloaded-virtual -Weffc++ -Wno-error=effc++)
    set(DEBUG_FLAGS ${DEBUG_FLAGS} -Wsign-conversion)
endif ()

set(RELEASE_FLAGS ${COMPILE_FLAGS} -O3 -flto -mtune=native -march=native)

add_subdirectory("${CMAKE_SOURCE_DIR}/Lib")
add_subdirectory("${CMAKE_SOURCE_DIR}/Assembler")

add_executable(LC3AS "${LIBRARY_SOURCE_FILES}" "${LIBRARY_HEADER_FILES}" "${ASSEMBLER_SOURCE_FILES}" "${ASSEMBLER_HEADER_FILES}")

target_include_directories(LC3AS PUBLIC "${LIBRARY_INCLUDE_DIRECTORIES}" "${ASSEMBLER_INCLUDE_DIRECTORIES}")
target_compile_options(LC3AS PUBLIC "$<$<CONFIG:DEBUG>:${DEBUG_FLAGS}>")
target_compile_options(LC3AS PUBLIC "$<$<CONFIG:RELEASE>:${RELEASE_FLAGS}>")

if (APPLE)
    if (NOT "${CMAKE_BUILD_TYPE}" STREQUAL "Debug")
        set(Boost_USE_STATIC_LIBS ON)
    endif ()
    find_package(Boost REQUIRED COMPONENTS filesystem system)
    target_link_libraries(LC3AS PUBLIC "${Boost_FILESYSTEM_LIBRARY}" "${Boost_SYSTEM_LIBRARY}")
    target_include_directories(LC3AS PUBLIC "${Boost_INCLUDE_DIR}")
elseif (UNIX)
    # GCC on linux requires this to use experimental/filesystem
    target_link_libraries(LC3AS PUBLIC stdc++fs)
endif ()

if (ALLOW_EXTRAS)
    target_compile_definitions(LC3AS PUBLIC INCLUDE_ADDONS)
endif ()
