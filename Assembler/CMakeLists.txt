
cmake_minimum_required ( VERSION 3.6 )
project ( LC3AS )

include_directories ( includes )

set ( SOURCE_FILES
      main.cpp
      )

add_subdirectory ( "Library" )

set ( DEBUG_FLAGS ${COMPILE_FLAGS} -O0 -g -pg -Werror -pedantic -Wshadow -Wnon-virtual-dtor -Wconversion )
set ( DEBUG_FLAGS ${DEBUG_FLAGS} -Wold-style-cast -Wcast-align -Wunused -Woverloaded-virtual -Weffc++ )
set ( DEBUG_FLAGS ${DEBUG_FLAGS} -Wsign-conversion -Wno-error=effc++ -Wno-error=sign-conversion -Wno-error=conversion )

add_executable ( LC3AS ${SOURCE_FILES} )
target_link_libraries ( LC3AS LC3Assembler )

target_compile_options ( ${PROJECT_NAME} PUBLIC "$<$<CONFIG:DEBUG>:${DEBUG_FLAGS}>" )
target_compile_options ( ${PROJECT_NAME} PUBLIC "$<$<CONFIG:RELEASE>:${RELEASE_FLAGS}>" )
