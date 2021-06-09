cmake_minimum_required(VERSION 3.20)

file(GLOB SOURCE_FILES *.cpp)
add_executable(formulae1_test
    ${PROJECT_SOURCE_DIR}/test/test.cpp
    ${SOURCE_FILES})

target_include_directories(formulae1_test
  PRIVATE
    ${PROJECT_SOURCE_DIR}/source)

target_link_libraries(formulae1_test
  PRIVATE
    formulae1)
