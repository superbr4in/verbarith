cmake_minimum_required(VERSION 3.13)

file(GLOB SOURCE_FILES *.cpp)

add_library(verbarith
  SHARED
    ${SOURCE_FILES})

target_include_directories(verbarith
  PUBLIC
    ${PROJECT_SOURCE_DIR}/include
  PRIVATE
    ${PROJECT_SOURCE_DIR}/source)

target_link_libraries(verbarith
  PUBLIC
    stimpak
  PRIVATE
    z3)
