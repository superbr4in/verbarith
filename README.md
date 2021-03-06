# Formulae1

C++20 Arithmetic Expression Library on [Z3](https://github.com/Z3Prover/z3) Booleans and Bit Vectors


## Prerequisites

Minimum requirements this project is guaranteed to work with:
- [LLVM Clang Compiler](https://clang.llvm.org/) (v11.0.0)
- [CMake](https://cmake.org/) (v3.13.4)
- [Z3 Theorem Prover](https://github.com/Z3Prover/z3) (v4.8.9), available by source through a git submodule


## Quickstart

Clone the repository and its submodule:
```sh
git clone https://github.com/superbr4in/formulae1.git
cd formulae1
git submodule update --init
```

Configure and compile the source code:
```sh
mkdir build
cd build
cmake .. -D"CMAKE_BUILD_TYPE=Release" -W"no-dev"
make formulae1 -j $(nproc)
```
Find `libformulae1.so` in `source/formulae1/`.

Run the tests (requires [Catch2](https://github.com/catchorg/Catch2)):
```sh
make formulae1_test
./test/formulae1/formulae1_test
```


## Project Integration

Use a `CMakeLists.txt` file to integrate the library into another project:
```cmake
# Older versions may work as well but are not tested
cmake_minimum_required(VERSION 3.13)
# Choose the directory this repository has been cloned into
add_subdirectory(formulae1)
...
# All header files will become available automatically
target_link_libraries(<some-project> PUBLIC formulae1)
```
See the official [CMake documentation](https://cmake.org/cmake/help/v3.13/) for further explanations and more options.
