cmake_minimum_required(VERSION 3.13)

file(GLOB_RECURSE SOURCE_FILES *.cpp)

add_executable(formulae1_test
    ${PROJECT_SOURCE_DIR}/test/main.cpp
    ${SOURCE_FILES})

target_link_libraries(formulae1_test
  PRIVATE
    formulae1)
