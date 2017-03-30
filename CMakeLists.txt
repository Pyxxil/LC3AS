cmake_minimum_required ( VERSION 3.6 )

set ( CMAKE_CXX_STANDARD 14 )

add_subdirectory ( "${CMAKE_SOURCE_DIR}/Assembler" )  # Builds the assembler
add_subdirectory ( "${CMAKE_SOURCE_DIR}/Simulator" )  # Builds the simulator
