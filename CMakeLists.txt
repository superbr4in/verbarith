cmake_minimum_required(VERSION 3.13)

project(formulae1 CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_EXPORT_COMPILE_COMMANDS OFF)

add_subdirectory(submodule)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_subdirectory(test)

if (CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pedantic -Wall -Wconversion -Werror -Wextra -Wfatal-errors -Wimplicit-fallthrough -Wno-gnu-zero-variadic-macro-arguments -Wshadow")
endif ()

add_subdirectory(source)
