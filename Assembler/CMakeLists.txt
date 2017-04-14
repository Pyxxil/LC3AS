
cmake_minimum_required ( VERSION 3.6 )
project ( LC3AS )

include_directories ( includes )

set ( SOURCE_FILES
      main.cpp
      )

add_subdirectory ( "Library" )

add_executable ( LC3AS ${SOURCE_FILES} )
target_link_libraries ( LC3AS LC3Assembler )
