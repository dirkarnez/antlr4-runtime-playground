cmake_minimum_required(VERSION 3.14)

# compiler must be 17
SET(CMAKE_CXX_STANDARD 17)
SET(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -static")

project("antlr4-runtime-playground" VERSION 1.0.0) # CICD should be consistent with this version 

set(ANTLR4_CPP_GENERATED_SRC_DIR src/antlr-generated)

set(ANTLR4_JAR_LOCATION ${PROJECT_SOURCE_DIR}/thirdparty/antlr/antlr-4.12.0-complete.jar)
file(DOWNLOAD "https://www.antlr.org/download/antlr-4.12.0-complete.jar" ${ANTLR4_JAR_LOCATION})

find_package(antlr4-generator CONFIG REQUIRED)
find_package(antlr4-runtime CONFIG REQUIRED)

# generate lexer
antlr4_generate(
  antlrcpptest_lexer
  ${CMAKE_CURRENT_SOURCE_DIR}/TLexer.g4
  LEXER
  FALSE
  FALSE
  "antlrcpptest"
)

# generate parser
antlr4_generate(
  antlrcpptest_parser
  ${CMAKE_CURRENT_SOURCE_DIR}/TParser.g4
  PARSER
  FALSE
  TRUE
  "antlrcpptest"
  "${ANTLR4_TOKEN_FILES_antlrcpptest_lexer}"
  "${ANTLR4_TOKEN_DIRECTORY_antlrcpptest_lexer}"
)

message(STATUS ${ANTLR4_INCLUDE_DIR})
message(STATUS ${ANTLR4_INCLUDE_DIR_antlrcpptest_lexer})
message(STATUS ${ANTLR4_INCLUDE_DIR_antlrcpptest_parser})

message(STATUS ${ANTLR4_TOKEN_FILES_antlrcpptest_lexer})
message(STATUS ${ANTLR4_TOKEN_DIRECTORY_antlrcpptest_lexer})

message(STATUS ${ANTLR4_SRC_FILES_antlrcpptest_lexer})
message(STATUS ${ANTLR4_SRC_FILES_antlrcpptest_parser})

# add directories for generated include files
include_directories(
  ${PROJECT_BINARY_DIR} 
  ${ANTLR4_INCLUDE_DIR}
  ${ANTLR4_INCLUDE_DIR_antlrcpptest_lexer}
  ${ANTLR4_INCLUDE_DIR_antlrcpptest_parser}
)

# add generated source files
add_executable(${PROJECT_NAME} 
  src/main.cpp 
  ${ANTLR4_SRC_FILES_antlrcpptest_lexer} 
  ${ANTLR4_SRC_FILES_antlrcpptest_parser}
)

# add required runtime library
add_dependencies(${PROJECT_NAME} antlr4_static)

target_link_libraries(${PROJECT_NAME} PRIVATE antlr4_static)