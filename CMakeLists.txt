cmake_minimum_required ( VERSION 3.6 )

project ( LC3_ASSEMBLER )

set ( CMAKE_CXX_STANDARD 14 )

set ( COMPILE_FLAGS -Wall -Wextra -Wpedantic )

if ( "${CMAKE_BUILD_TYPE}" STREQUAL "Debug" )
        set ( CMAKE_VERBOSE_MAKEFILE ON )
endif ()

set ( LIBRARY_SOURCE_FILES "" )
set ( LIBRARY_INCLUDE_DIRECTORIES "" )

set ( ASSEMBLER_SOURCE_FILES "" )
set ( ASSEMBLER_INCLUDE_DIRECTORIES "" )

set ( DEBUG_FLAGS ${COMPILE_FLAGS} -O0 -g -pg -Werror -pedantic -Wshadow -Wnon-virtual-dtor -Wconversion )
set ( DEBUG_FLAGS ${DEBUG_FLAGS} -Wold-style-cast -Wcast-align -Wunused -Woverloaded-virtual -Weffc++ )
set ( DEBUG_FLAGS ${DEBUG_FLAGS} -Wsign-conversion -Wno-error=effc++ )

set ( RELEASE_FLAGS ${COMPILE_FLAGS} -O3 -flto -mtune=native -march=native )

add_subdirectory ( "${CMAKE_SOURCE_DIR}/Lib" )
add_subdirectory ( "${CMAKE_SOURCE_DIR}/Assembler" )

add_executable ( LC3AS "${LIBRARY_SOURCE_FILES}" "${ASSEMBLER_SOURCE_FILES}" )

target_include_directories ( LC3AS PUBLIC "${LIBRARY_INCLUDE_DIRECTORIES}" "${ASSEMBLER_INCLUDE_DIRECTORIES}" )
