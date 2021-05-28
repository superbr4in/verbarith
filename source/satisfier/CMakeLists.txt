cmake_minimum_required(VERSION 3.13)

file(GLOB SOURCE_FILES *.cpp)

add_executable(satisfier
    ${SOURCE_FILES})

target_link_libraries(satisfier
  PRIVATE
    formulae1)
