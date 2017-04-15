cmake_minimum_required ( VERSION 3.6 )
project ( LC3_simulator )

set ( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Werror -Wpedantic" )

set ( SIMULATOR_SOURCE_FILES
      source/Simulator.cpp
      main.cpp
      )

set ( SIMULATOR_HEADER_FILES
      includes/Simulator.hpp
      )

set ( DEFINITIONS "PROJECT=\"${PROJECT_NAME}\"" )

find_library ( Curses curses )

add_executable ( LC3_simulator ${SIMULATOR_SOURCE_FILES} ${SIMULATOR_HEADER_FILES} )
target_link_libraries ( LC3_simulator ${Curses} LC3Assembler )

target_compile_options ( ${PROJECT_NAME} PUBLIC "$<$<CONFIG:DEBUG>:${DEBUG_FLAGS}>" )
target_compile_options ( ${PROJECT_NAME} PUBLIC "$<$<CONFIG:RELEASE>:${RELEASE_FLAGS}>" )

target_compile_definitions ( ${PROJECT_NAME} PUBLIC "${DEFINITIONS}" )
