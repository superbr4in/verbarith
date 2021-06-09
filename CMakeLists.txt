cmake_minimum_required(VERSION 3.13)

project(formulae1 CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_EXPORT_COMPILE_COMMANDS OFF)

add_subdirectory(submodule)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_subdirectory(test)

set(WARNINGS_LIST
  # Treat all warnings as errors
  -Werror
  # Abort on the first error
  -Wfatal-errors
  # Common warnings
  -Wall
  -Wextra
  # Prohibit non-standard C++
  -Wpedantic
  # Additional warnings
  -Wcast-align
  -Wconversion
  -Wdouble-promotion
  -Wimplicit-fallthrough
  -Wmisleading-indentation
  -Wnon-virtual-dtor
  -Wnull-dereference
  -Wold-style-cast
  -Woverloaded-virtual
  -Wshadow
  -Wsign-conversion
  -Wunused
  # Disabled warnings
  -Wno-gnu-zero-variadic-macro-arguments)
string(JOIN " " WARNINGS ${WARNINGS_LIST})
string(APPEND CMAKE_CXX_FLAGS ${WARNINGS})

add_subdirectory(source)
