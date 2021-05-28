cmake_minimum_required(VERSION 3.13)

file(GLOB SOURCE_FILES *.cpp)

add_library(formulae1
  SHARED
    ${SOURCE_FILES})

target_include_directories(formulae1
  PUBLIC
    ${PROJECT_SOURCE_DIR}/include)

target_link_libraries(formulae1
  PRIVATE
    z3)
