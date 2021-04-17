cmake_minimum_required(VERSION 3.13)

project(verbarith CXX)
set(CMAKE_CXX_STANDARD 20)

add_subdirectory(submodule)

if (CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pedantic -Wall -Wconversion -Werror -Wextra -Wfatal-errors -Wimplicit-fallthrough -Wno-gnu-zero-variadic-macro-arguments -Wshadow")
endif ()

add_subdirectory(source)
add_subdirectory(test)
