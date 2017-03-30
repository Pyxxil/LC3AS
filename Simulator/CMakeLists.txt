cmake_minimum_required ( VERSION 3.6 )
project ( LC3_simulator )

set ( CMAKE_CXX_STANDARD 14 )

add_definitions ( "-DPROJECT=\"${PROJECT_NAME}\"" )

set ( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Werror -Wpedantic" )

set ( SIMULATOR_SOURCE_FILES
      source/Simulator.cpp
      main.cpp
      )

set ( SIMULATOR_HEADER_FILES
      includes/Simulator.hpp
      )

set ( ASSEMBLER_SOURCE_FILES
      Assembler/source/Assembler.cpp
      Assembler/source/Token.cpp
      PARENT_SCOPE )

set ( ASSEMBLER_HEADER_FILES
      Assembler/includes/Assembler.hpp
      Assembler/includes/Token.hpp
      PARENT_SCOPE )

set ( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Wpedantic" )

if ( "${CMAKE_BUILD_TYPE}" STREQUAL "Debug" )
    set ( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -pg -Werror -pedantic -Wshadow -Wnon-virtual-dtor" )
    set ( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wold-style-cast -Wcast-align -Wunused -Woverloaded-virtual" )
    set ( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wconversion -Wsign-conversion -Weffc++ -Wno-error=effc++" )
    set ( CMAKE_VERBOSE_MAKEFILE ON )
endif ()

find_library ( Curses curses )

add_executable ( LC3_simulator ${SIMULATOR_SOURCE_FILES} ${SIMULATOR_HEADER_FILES} ${ASSEMBLER_SOURCE_FILES} ${ASSEMBLER_HEADER_FILES} )
target_link_libraries ( LC3_simulator ${Curses} )
