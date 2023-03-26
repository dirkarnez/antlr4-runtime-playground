cmake_minimum_required(VERSION 3.14)

# compiler must be 17
SET(CMAKE_CXX_STANDARD 17)
SET(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -static")

project("antlr4-runtime-playground" VERSION 1.0.0) # CICD should be consistent with this version 

set(ANTLR4_JAR_LOCATION ${PROJECT_SOURCE_DIR}/thirdparty/antlr/antlr-4.12.0-complete.jar)
file(DOWNLOAD "https://www.antlr.org/download/antlr-4.12.0-complete.jar" ${ANTLR4_JAR_LOCATION})

find_package(antlr4-generator CONFIG REQUIRED)
find_package(antlr4-runtime CONFIG REQUIRED)

# generate lexer
antlr4_generate(
  antlrcpptest_lexer
  ${CMAKE_CURRENT_SOURCE_DIR}/ZLexer.g4
  LEXER
  FALSE
  FALSE
  "antlrcpptest"
)

# generate parser
antlr4_generate(
  antlrcpptest_parser
  ${CMAKE_CURRENT_SOURCE_DIR}/ZParser.g4
  PARSER
  TRUE
  TRUE
  "antlrcpptest"
  "${ANTLR4_TOKEN_FILES_antlrcpptest_lexer}"
  "${ANTLR4_TOKEN_DIRECTORY_antlrcpptest_lexer}"
)

message(STATUS ${ANTLR4_GENERATED_SRC_DIR})
message(STATUS ${ANTLR4_CPP_GENERATED_SRC_DIR})

message(STATUS ${ANTLR4_INCLUDE_DIR})
message(STATUS ${ANTLR4_INCLUDE_DIR_antlrcpptest_lexer})
message(STATUS ${ANTLR4_INCLUDE_DIR_antlrcpptest_parser})

message(STATUS ${ANTLR4_TOKEN_FILES_antlrcpptest_lexer})
message(STATUS ${ANTLR4_TOKEN_DIRECTORY_antlrcpptest_lexer})

message(STATUS ${ANTLR4_SRC_FILES_antlrcpptest_lexer})
message(STATUS ${ANTLR4_SRC_FILES_antlrcpptest_parser})

# add directories for generated include files
include_directories(
  ${ANTLR4_INCLUDE_DIR}
  ${ANTLR4_INCLUDE_DIR_antlrcpptest_lexer}
  ${ANTLR4_INCLUDE_DIR_antlrcpptest_parser}
)

# add generated source files
add_executable(${PROJECT_NAME} 
  "src/main.cpp"
  "src/common/logger.cpp"
  "src/common/util.cpp"
  "src/compiler/compiler.cpp"
  "src/compiler/bytecode_generator.cpp"
  "src/compiler/ast/expresssion.cpp"
  "src/compiler/ast/function.cpp"
  "src/compiler/ast/if.cpp"
  "src/compiler/ast/loop.cpp"
  "src/compiler/ast/program.cpp"
  "src/compiler/ast/statement.cpp"
  "src/compiler/ast/type.cpp"
  "src/compiler/ast/variable.cpp"
  "src/compiler/op/op.cpp"
  "src/compiler/program/program.cpp"
  "src/compiler/type/extractor/context_chain.cpp"
  "src/compiler/type/extractor/type_extractor.cpp"
  "src/compiler/type/extractor/type_helper.cpp"
  "src/compiler/type/type_repository.cpp"
  "src/compiler/type/type.cpp"
  ${ANTLR4_SRC_FILES_antlrcpptest_lexer} 
  ${ANTLR4_SRC_FILES_antlrcpptest_parser}
)

target_include_directories(${PROJECT_NAME} PRIVATE 
  "include"
)

# add required runtime library
add_dependencies(${PROJECT_NAME} antlr4_static)

target_link_libraries(${PROJECT_NAME} PRIVATE antlr4_static)