cmake_minimum_required ( VERSION 3.6 )

set ( CMAKE_CXX_STANDARD 14 )

set ( COMPILE_FLAGS -Wall -Wextra -Wpedantic )

if ( "${CMAKE_BUILD_TYPE}" STREQUAL "Debug" )
        set ( CMAKE_VERBOSE_MAKEFILE ON )
endif ()

set ( DEBUG_FLAGS ${COMPILE_FLAGS} -O0 -g -pg -Werror -pedantic -Wshadow -Wnon-virtual-dtor -Wconversion )
set ( DEBUG_FLAGS ${DEBUG_FLAGS} -Wold-style-cast -Wcast-align -Wunused -Woverloaded-virtual -Weffc++ )
set ( DEBUG_FLAGS ${DEBUG_FLAGS} -Wsign-conversion -Wno-error=effc++ )

set ( RELEASE_FLAGS ${COMPILE_FLAGS} -Os -flto -mtune=native )

add_subdirectory ( "${CMAKE_SOURCE_DIR}/Assembler" )  # Builds the assembler
add_subdirectory ( "${CMAKE_SOURCE_DIR}/Simulator" )  # Builds the simulator
