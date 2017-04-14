cmake_minimum_required ( VERSION 3.6 )

set ( CMAKE_CXX_STANDARD 14 )

set ( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Wpedantic" )

if ( "${CMAKE_BUILD_TYPE}" STREQUAL "Debug" )
        set ( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O0 -g -pg -Werror -pedantic -Wshadow -Wnon-virtual-dtor" )
        set ( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wold-style-cast -Wcast-align -Wunused -Woverloaded-virtual" )
        set ( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wconversion -Wsign-conversion -Weffc++ -Wno-error=effc++" )
        set ( CMAKE_VERBOSE_MAKEFILE ON )
else ()
        set ( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3 -flto -mtune=native" )
endif ()

add_subdirectory ( "${CMAKE_SOURCE_DIR}/Assembler" )  # Builds the assembler
add_subdirectory ( "${CMAKE_SOURCE_DIR}/Simulator" )  # Builds the simulator
