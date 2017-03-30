cmake_minimum_required ( VERSION 3.6 )
project ( LC3AS )

set ( CMAKE_CXX_STANDARD 14 )

add_definitions ( "-DPROJECT=\"${PROJECT_NAME}\"" )

set ( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Werror -Wpedantic" )

set ( SOURCE_FILES
      main.cpp
      )

set ( ASSEMBLER_SOURCE_FILES
      source/Assembler.cpp
      source/Token.cpp
      )

set ( ASSEMBLER_HEADER_FILES
      includes/Assembler.hpp
      includes/Token.hpp
      )

set ( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Wpedantic" )

if ( "${CMAKE_BUILD_TYPE}" STREQUAL "Debug" )
    set ( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -pg -Werror -pedantic -Wshadow -Wnon-virtual-dtor" )
    set ( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wold-style-cast -Wcast-align -Wunused -Woverloaded-virtual" )
    set ( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wconversion -Wsign-conversion -Weffc++ -Wno-error=effc++" )
    set ( CMAKE_VERBOSE_MAKEFILE ON )
endif ()

add_executable ( LC3AS ${SOURCE_FILES} ${ASSEMBLER_SOURCE_FILES} ${ASSEMBLER_HEADER_FILES} )
