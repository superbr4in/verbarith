cmake_minimum_required(VERSION 3.20)

project(formulae1 CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# ---------------------------------------------------------------------------- #

add_subdirectory(submodule)

# ---------------------------------------------------------------------------- #

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(ADDITIONAL_CXX_FLAGS_DEBUG_LIST
  # Produce debugging information for GDB
  -ggdb
  # Apply debugging optimizations
  -Og
  # Enable address sanitizer
  -fsanitize=address)
string(JOIN " " ADDITIONAL_CXX_FLAGS_DEBUG ${ADDITIONAL_CXX_FLAGS_DEBUG_LIST})
string(PREPEND       CMAKE_CXX_FLAGS_DEBUG ${ADDITIONAL_CXX_FLAGS_DEBUG} " ")

set(ADDITIONAL_CXX_FLAGS_LIST
  # Abort on the first error
  -Wfatal-errors
  # Common warnings
  -Wall
  -Wextra
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
  -Wpedantic
  -Wshadow
  -Wsign-conversion
  -Wunused
  # Disabled warnings
  -Wno-gnu-zero-variadic-macro-arguments)
string(JOIN " " ADDITIONAL_CXX_FLAGS ${ADDITIONAL_CXX_FLAGS_LIST})
string(PREPEND       CMAKE_CXX_FLAGS ${ADDITIONAL_CXX_FLAGS} " ")

# ---------------------------------------------------------------------------- #

add_subdirectory(test)

# ---------------------------------------------------------------------------- #

# Treat all warnings as errors
string(PREPEND CMAKE_CXX_FLAGS -Werror " ")

# ---------------------------------------------------------------------------- #

add_subdirectory(source)

# ---------------------------------------------------------------------------- #
