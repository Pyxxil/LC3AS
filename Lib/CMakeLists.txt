
set ( LEXER_SOURCE_FILES
      ${CMAKE_CURRENT_SOURCE_DIR}/Lexer/Lexer.cpp
      )

set ( LEXER_HEADER_FILES
      ${CMAKE_CURRENT_SOURCE_DIR}/Includes/Lexer.hpp
      )

set ( PARSER_SOURCE_FILES
      ${CMAKE_CURRENT_SOURCE_DIR}/Parser/Parser.cpp
      )

set ( PARSER_HEADER_FILES
      ${CMAKE_CURRENT_SOURCE_DIR}/Includes/Parser.hpp
      )

set ( DIAGNOSTICS_SOURCE_FILES
      ${CMAKE_CURRENT_SOURCE_DIR}/Diagnostics/Diagnostics.cpp
      ${CMAKE_CURRENT_SOURCE_DIR}/Diagnostics/Context.cpp
      )

set ( DIAGNOSTICS_HEADER_FILES
      ${CMAKE_CURRENT_SOURCE_DIR}/Includes/Diagnostics.hpp
      ${CMAKE_CURRENT_SOURCE_DIR}/Includes/Context.hpp
      )

set ( CONFIGURATION_SOURCE_FILES
      ${CMAKE_CURRENT_SOURCE_DIR}/Config/Configuration.cpp
      )

set ( CONFIGURATION_HEADER_FILES
      ${CMAKE_CURRENT_SOURCE_DIR}/Includes/Configuration.hpp
      )

set ( LIBRARY_SOURCE_FILES
      "${LIBRARY_SOURCE_FILES}"
      "${PARSER_SOURCE_FILES}"
      "${LEXER_SOURCE_FILES}"
      "${DIAGNOSTICS_SOURCE_FILES}"
      "${CONFIGURATION_SOURCE_FILES}"
      PARENT_SCOPE
      )

set ( LIBRARY_HEADER_FILES
      "${LIBRARY_HEADER_FILES}"
      "${PARSER_HEADER_FILES}"
      "${LEXER_HEADER_FILES}"
      "${DIAGNOSTICS_HEADER_FILES}"
      "${CONFIGURATION_HEADER_FILES}"
      "${CMAKE_CURRENT_SOURCE_DIR}/Includes/Console.hpp"
      PARENT_SCOPE
      )

set ( LIBRARY_INCLUDE_DIRECTORIES
      "${LIBRARY_INCLUDE_DIRECTORIES}"
      ${CMAKE_CURRENT_SOURCE_DIR}/Includes/
      PARENT_SCOPE
      )
