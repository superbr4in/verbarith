#pragma once

#include <concepts>
#include <cstddef>

#include "preprocessor.hpp"

// clang-format off

#define TYPE(T) TYPE_##T

#define LOOP_TYPES_0(action, ...) LOOP_12_0(action, __VA_ARGS__)
#define LOOP_TYPES_1(action, ...) LOOP_12_1(action, __VA_ARGS__)

#define TYPE_SIZE_DIVIDE(T, U) TYPE_SIZE_DIVIDE_##T##_##U

#define LOOP_TYPE_SIZE_DIVIDE_0(T, U, action, ...) LOOP_TYPE_SIZE_DIVIDE_##T##_##U(0, action, __VA_ARGS__)
#define LOOP_TYPE_SIZE_DIVIDE_1(T, U, action, ...) LOOP_TYPE_SIZE_DIVIDE_##T##_##U(1, action, __VA_ARGS__)
#define LOOP_TYPE_SIZE_DIVIDE_2(T, U, action, ...) LOOP_TYPE_SIZE_DIVIDE_##T##_##U(2, action, __VA_ARGS__)

#define IF_TYPE_SIZE_EQUAL(T, U, action) IF_TYPE_SIZE_EQUAL_##T##_##U(action)

#define IF_TYPE_SIZE_DIVIDABLE(T, U, action) IF_TYPE_SIZE_DIVIDABLE_##T##_##U(action)

#define TYPE_0  std::byte
#define TYPE_1                     char
#define TYPE_2    signed           char
#define TYPE_3  unsigned           char
#define TYPE_4    signed short     int
#define TYPE_5  unsigned short     int
#define TYPE_6    signed           int
#define TYPE_7  unsigned           int
#define TYPE_8    signed long      int
#define TYPE_9  unsigned long      int
#define TYPE_10   signed long long int
#define TYPE_11 unsigned long long int

#define TYPE_SIZE_DIVIDE_0_0   1
#define TYPE_SIZE_DIVIDE_0_1   1
#define TYPE_SIZE_DIVIDE_0_2   1
#define TYPE_SIZE_DIVIDE_0_3   1
#define TYPE_SIZE_DIVIDE_0_4   0
#define TYPE_SIZE_DIVIDE_0_5   0
#define TYPE_SIZE_DIVIDE_0_6   0
#define TYPE_SIZE_DIVIDE_0_7   0
#define TYPE_SIZE_DIVIDE_0_8   0
#define TYPE_SIZE_DIVIDE_0_9   0
#define TYPE_SIZE_DIVIDE_0_10  0
#define TYPE_SIZE_DIVIDE_0_11  0
#define TYPE_SIZE_DIVIDE_1_0   1
#define TYPE_SIZE_DIVIDE_1_1   1
#define TYPE_SIZE_DIVIDE_1_2   1
#define TYPE_SIZE_DIVIDE_1_3   1
#define TYPE_SIZE_DIVIDE_1_4   0
#define TYPE_SIZE_DIVIDE_1_5   0
#define TYPE_SIZE_DIVIDE_1_6   0
#define TYPE_SIZE_DIVIDE_1_7   0
#define TYPE_SIZE_DIVIDE_1_8   0
#define TYPE_SIZE_DIVIDE_1_9   0
#define TYPE_SIZE_DIVIDE_1_10  0
#define TYPE_SIZE_DIVIDE_1_11  0
#define TYPE_SIZE_DIVIDE_2_0   1
#define TYPE_SIZE_DIVIDE_2_1   1
#define TYPE_SIZE_DIVIDE_2_2   1
#define TYPE_SIZE_DIVIDE_2_3   1
#define TYPE_SIZE_DIVIDE_2_4   0
#define TYPE_SIZE_DIVIDE_2_5   0
#define TYPE_SIZE_DIVIDE_2_6   0
#define TYPE_SIZE_DIVIDE_2_7   0
#define TYPE_SIZE_DIVIDE_2_8   0
#define TYPE_SIZE_DIVIDE_2_9   0
#define TYPE_SIZE_DIVIDE_2_10  0
#define TYPE_SIZE_DIVIDE_2_11  0
#define TYPE_SIZE_DIVIDE_3_0   1
#define TYPE_SIZE_DIVIDE_3_1   1
#define TYPE_SIZE_DIVIDE_3_2   1
#define TYPE_SIZE_DIVIDE_3_3   1
#define TYPE_SIZE_DIVIDE_3_4   0
#define TYPE_SIZE_DIVIDE_3_5   0
#define TYPE_SIZE_DIVIDE_3_6   0
#define TYPE_SIZE_DIVIDE_3_7   0
#define TYPE_SIZE_DIVIDE_3_8   0
#define TYPE_SIZE_DIVIDE_3_9   0
#define TYPE_SIZE_DIVIDE_3_10  0
#define TYPE_SIZE_DIVIDE_3_11  0
#define TYPE_SIZE_DIVIDE_4_0   2
#define TYPE_SIZE_DIVIDE_4_1   2
#define TYPE_SIZE_DIVIDE_4_2   2
#define TYPE_SIZE_DIVIDE_4_3   2
#define TYPE_SIZE_DIVIDE_4_4   1
#define TYPE_SIZE_DIVIDE_4_5   1
#define TYPE_SIZE_DIVIDE_4_6   0
#define TYPE_SIZE_DIVIDE_4_7   0
#define TYPE_SIZE_DIVIDE_4_8   0
#define TYPE_SIZE_DIVIDE_4_9   0
#define TYPE_SIZE_DIVIDE_4_10  0
#define TYPE_SIZE_DIVIDE_4_11  0
#define TYPE_SIZE_DIVIDE_5_0   2
#define TYPE_SIZE_DIVIDE_5_1   2
#define TYPE_SIZE_DIVIDE_5_2   2
#define TYPE_SIZE_DIVIDE_5_3   2
#define TYPE_SIZE_DIVIDE_5_4   1
#define TYPE_SIZE_DIVIDE_5_5   1
#define TYPE_SIZE_DIVIDE_5_6   0
#define TYPE_SIZE_DIVIDE_5_7   0
#define TYPE_SIZE_DIVIDE_5_8   0
#define TYPE_SIZE_DIVIDE_5_9   0
#define TYPE_SIZE_DIVIDE_5_10  0
#define TYPE_SIZE_DIVIDE_5_11  0
#define TYPE_SIZE_DIVIDE_6_0   4
#define TYPE_SIZE_DIVIDE_6_1   4
#define TYPE_SIZE_DIVIDE_6_2   4
#define TYPE_SIZE_DIVIDE_6_3   4
#define TYPE_SIZE_DIVIDE_6_4   2
#define TYPE_SIZE_DIVIDE_6_5   2
#define TYPE_SIZE_DIVIDE_6_6   1
#define TYPE_SIZE_DIVIDE_6_7   1
#define TYPE_SIZE_DIVIDE_6_8   0
#define TYPE_SIZE_DIVIDE_6_9   0
#define TYPE_SIZE_DIVIDE_6_10  0
#define TYPE_SIZE_DIVIDE_6_11  0
#define TYPE_SIZE_DIVIDE_7_0   4
#define TYPE_SIZE_DIVIDE_7_1   4
#define TYPE_SIZE_DIVIDE_7_2   4
#define TYPE_SIZE_DIVIDE_7_3   4
#define TYPE_SIZE_DIVIDE_7_4   2
#define TYPE_SIZE_DIVIDE_7_5   2
#define TYPE_SIZE_DIVIDE_7_6   1
#define TYPE_SIZE_DIVIDE_7_7   1
#define TYPE_SIZE_DIVIDE_7_8   0
#define TYPE_SIZE_DIVIDE_7_9   0
#define TYPE_SIZE_DIVIDE_7_10  0
#define TYPE_SIZE_DIVIDE_7_11  0
#define TYPE_SIZE_DIVIDE_8_0   8
#define TYPE_SIZE_DIVIDE_8_1   8
#define TYPE_SIZE_DIVIDE_8_2   8
#define TYPE_SIZE_DIVIDE_8_3   8
#define TYPE_SIZE_DIVIDE_8_4   4
#define TYPE_SIZE_DIVIDE_8_5   4
#define TYPE_SIZE_DIVIDE_8_6   2
#define TYPE_SIZE_DIVIDE_8_7   2
#define TYPE_SIZE_DIVIDE_8_8   1
#define TYPE_SIZE_DIVIDE_8_9   1
#define TYPE_SIZE_DIVIDE_8_10  1
#define TYPE_SIZE_DIVIDE_8_11  1
#define TYPE_SIZE_DIVIDE_9_0   8
#define TYPE_SIZE_DIVIDE_9_1   8
#define TYPE_SIZE_DIVIDE_9_2   8
#define TYPE_SIZE_DIVIDE_9_3   8
#define TYPE_SIZE_DIVIDE_9_4   4
#define TYPE_SIZE_DIVIDE_9_5   4
#define TYPE_SIZE_DIVIDE_9_6   2
#define TYPE_SIZE_DIVIDE_9_7   2
#define TYPE_SIZE_DIVIDE_9_8   1
#define TYPE_SIZE_DIVIDE_9_9   1
#define TYPE_SIZE_DIVIDE_9_10  1
#define TYPE_SIZE_DIVIDE_9_11  1
#define TYPE_SIZE_DIVIDE_10_0  8
#define TYPE_SIZE_DIVIDE_10_1  8
#define TYPE_SIZE_DIVIDE_10_2  8
#define TYPE_SIZE_DIVIDE_10_3  8
#define TYPE_SIZE_DIVIDE_10_4  4
#define TYPE_SIZE_DIVIDE_10_5  4
#define TYPE_SIZE_DIVIDE_10_6  2
#define TYPE_SIZE_DIVIDE_10_7  2
#define TYPE_SIZE_DIVIDE_10_8  1
#define TYPE_SIZE_DIVIDE_10_9  1
#define TYPE_SIZE_DIVIDE_10_10 1
#define TYPE_SIZE_DIVIDE_10_11 1
#define TYPE_SIZE_DIVIDE_11_0  8
#define TYPE_SIZE_DIVIDE_11_1  8
#define TYPE_SIZE_DIVIDE_11_2  8
#define TYPE_SIZE_DIVIDE_11_3  8
#define TYPE_SIZE_DIVIDE_11_4  4
#define TYPE_SIZE_DIVIDE_11_5  4
#define TYPE_SIZE_DIVIDE_11_6  2
#define TYPE_SIZE_DIVIDE_11_7  2
#define TYPE_SIZE_DIVIDE_11_8  1
#define TYPE_SIZE_DIVIDE_11_9  1
#define TYPE_SIZE_DIVIDE_11_10 1
#define TYPE_SIZE_DIVIDE_11_11 1

#define LOOP_TYPE_SIZE_DIVIDE_0_0(  nested, action, ...) LOOP_##nested(1, action, __VA_ARGS__)
#define LOOP_TYPE_SIZE_DIVIDE_0_1(  nested, action, ...) LOOP_##nested(1, action, __VA_ARGS__)
#define LOOP_TYPE_SIZE_DIVIDE_0_2(  nested, action, ...) LOOP_##nested(1, action, __VA_ARGS__)
#define LOOP_TYPE_SIZE_DIVIDE_0_3(  nested, action, ...) LOOP_##nested(1, action, __VA_ARGS__)
#define LOOP_TYPE_SIZE_DIVIDE_0_4(  nested, action, ...) LOOP_##nested(0, action, __VA_ARGS__)
#define LOOP_TYPE_SIZE_DIVIDE_0_5(  nested, action, ...) LOOP_##nested(0, action, __VA_ARGS__)
#define LOOP_TYPE_SIZE_DIVIDE_0_6(  nested, action, ...) LOOP_##nested(0, action, __VA_ARGS__)
#define LOOP_TYPE_SIZE_DIVIDE_0_7(  nested, action, ...) LOOP_##nested(0, action, __VA_ARGS__)
#define LOOP_TYPE_SIZE_DIVIDE_0_8(  nested, action, ...) LOOP_##nested(0, action, __VA_ARGS__)
#define LOOP_TYPE_SIZE_DIVIDE_0_9(  nested, action, ...) LOOP_##nested(0, action, __VA_ARGS__)
#define LOOP_TYPE_SIZE_DIVIDE_0_10( nested, action, ...) LOOP_##nested(0, action, __VA_ARGS__)
#define LOOP_TYPE_SIZE_DIVIDE_0_11( nested, action, ...) LOOP_##nested(0, action, __VA_ARGS__)
#define LOOP_TYPE_SIZE_DIVIDE_1_0(  nested, action, ...) LOOP_##nested(1, action, __VA_ARGS__)
#define LOOP_TYPE_SIZE_DIVIDE_1_1(  nested, action, ...) LOOP_##nested(1, action, __VA_ARGS__)
#define LOOP_TYPE_SIZE_DIVIDE_1_2(  nested, action, ...) LOOP_##nested(1, action, __VA_ARGS__)
#define LOOP_TYPE_SIZE_DIVIDE_1_3(  nested, action, ...) LOOP_##nested(1, action, __VA_ARGS__)
#define LOOP_TYPE_SIZE_DIVIDE_1_4(  nested, action, ...) LOOP_##nested(0, action, __VA_ARGS__)
#define LOOP_TYPE_SIZE_DIVIDE_1_5(  nested, action, ...) LOOP_##nested(0, action, __VA_ARGS__)
#define LOOP_TYPE_SIZE_DIVIDE_1_6(  nested, action, ...) LOOP_##nested(0, action, __VA_ARGS__)
#define LOOP_TYPE_SIZE_DIVIDE_1_7(  nested, action, ...) LOOP_##nested(0, action, __VA_ARGS__)
#define LOOP_TYPE_SIZE_DIVIDE_1_8(  nested, action, ...) LOOP_##nested(0, action, __VA_ARGS__)
#define LOOP_TYPE_SIZE_DIVIDE_1_9(  nested, action, ...) LOOP_##nested(0, action, __VA_ARGS__)
#define LOOP_TYPE_SIZE_DIVIDE_1_10( nested, action, ...) LOOP_##nested(0, action, __VA_ARGS__)
#define LOOP_TYPE_SIZE_DIVIDE_1_11( nested, action, ...) LOOP_##nested(0, action, __VA_ARGS__)
#define LOOP_TYPE_SIZE_DIVIDE_2_0(  nested, action, ...) LOOP_##nested(1, action, __VA_ARGS__)
#define LOOP_TYPE_SIZE_DIVIDE_2_1(  nested, action, ...) LOOP_##nested(1, action, __VA_ARGS__)
#define LOOP_TYPE_SIZE_DIVIDE_2_2(  nested, action, ...) LOOP_##nested(1, action, __VA_ARGS__)
#define LOOP_TYPE_SIZE_DIVIDE_2_3(  nested, action, ...) LOOP_##nested(1, action, __VA_ARGS__)
#define LOOP_TYPE_SIZE_DIVIDE_2_4(  nested, action, ...) LOOP_##nested(0, action, __VA_ARGS__)
#define LOOP_TYPE_SIZE_DIVIDE_2_5(  nested, action, ...) LOOP_##nested(0, action, __VA_ARGS__)
#define LOOP_TYPE_SIZE_DIVIDE_2_6(  nested, action, ...) LOOP_##nested(0, action, __VA_ARGS__)
#define LOOP_TYPE_SIZE_DIVIDE_2_7(  nested, action, ...) LOOP_##nested(0, action, __VA_ARGS__)
#define LOOP_TYPE_SIZE_DIVIDE_2_8(  nested, action, ...) LOOP_##nested(0, action, __VA_ARGS__)
#define LOOP_TYPE_SIZE_DIVIDE_2_9(  nested, action, ...) LOOP_##nested(0, action, __VA_ARGS__)
#define LOOP_TYPE_SIZE_DIVIDE_2_10( nested, action, ...) LOOP_##nested(0, action, __VA_ARGS__)
#define LOOP_TYPE_SIZE_DIVIDE_2_11( nested, action, ...) LOOP_##nested(0, action, __VA_ARGS__)
#define LOOP_TYPE_SIZE_DIVIDE_3_0(  nested, action, ...) LOOP_##nested(1, action, __VA_ARGS__)
#define LOOP_TYPE_SIZE_DIVIDE_3_1(  nested, action, ...) LOOP_##nested(1, action, __VA_ARGS__)
#define LOOP_TYPE_SIZE_DIVIDE_3_2(  nested, action, ...) LOOP_##nested(1, action, __VA_ARGS__)
#define LOOP_TYPE_SIZE_DIVIDE_3_3(  nested, action, ...) LOOP_##nested(1, action, __VA_ARGS__)
#define LOOP_TYPE_SIZE_DIVIDE_3_4(  nested, action, ...) LOOP_##nested(0, action, __VA_ARGS__)
#define LOOP_TYPE_SIZE_DIVIDE_3_5(  nested, action, ...) LOOP_##nested(0, action, __VA_ARGS__)
#define LOOP_TYPE_SIZE_DIVIDE_3_6(  nested, action, ...) LOOP_##nested(0, action, __VA_ARGS__)
#define LOOP_TYPE_SIZE_DIVIDE_3_7(  nested, action, ...) LOOP_##nested(0, action, __VA_ARGS__)
#define LOOP_TYPE_SIZE_DIVIDE_3_8(  nested, action, ...) LOOP_##nested(0, action, __VA_ARGS__)
#define LOOP_TYPE_SIZE_DIVIDE_3_9(  nested, action, ...) LOOP_##nested(0, action, __VA_ARGS__)
#define LOOP_TYPE_SIZE_DIVIDE_3_10( nested, action, ...) LOOP_##nested(0, action, __VA_ARGS__)
#define LOOP_TYPE_SIZE_DIVIDE_3_11( nested, action, ...) LOOP_##nested(0, action, __VA_ARGS__)
#define LOOP_TYPE_SIZE_DIVIDE_4_0(  nested, action, ...) LOOP_##nested(2, action, __VA_ARGS__)
#define LOOP_TYPE_SIZE_DIVIDE_4_1(  nested, action, ...) LOOP_##nested(2, action, __VA_ARGS__)
#define LOOP_TYPE_SIZE_DIVIDE_4_2(  nested, action, ...) LOOP_##nested(2, action, __VA_ARGS__)
#define LOOP_TYPE_SIZE_DIVIDE_4_3(  nested, action, ...) LOOP_##nested(2, action, __VA_ARGS__)
#define LOOP_TYPE_SIZE_DIVIDE_4_4(  nested, action, ...) LOOP_##nested(1, action, __VA_ARGS__)
#define LOOP_TYPE_SIZE_DIVIDE_4_5(  nested, action, ...) LOOP_##nested(1, action, __VA_ARGS__)
#define LOOP_TYPE_SIZE_DIVIDE_4_6(  nested, action, ...) LOOP_##nested(0, action, __VA_ARGS__)
#define LOOP_TYPE_SIZE_DIVIDE_4_7(  nested, action, ...) LOOP_##nested(0, action, __VA_ARGS__)
#define LOOP_TYPE_SIZE_DIVIDE_4_8(  nested, action, ...) LOOP_##nested(0, action, __VA_ARGS__)
#define LOOP_TYPE_SIZE_DIVIDE_4_9(  nested, action, ...) LOOP_##nested(0, action, __VA_ARGS__)
#define LOOP_TYPE_SIZE_DIVIDE_4_10( nested, action, ...) LOOP_##nested(0, action, __VA_ARGS__)
#define LOOP_TYPE_SIZE_DIVIDE_4_11( nested, action, ...) LOOP_##nested(0, action, __VA_ARGS__)
#define LOOP_TYPE_SIZE_DIVIDE_5_0(  nested, action, ...) LOOP_##nested(2, action, __VA_ARGS__)
#define LOOP_TYPE_SIZE_DIVIDE_5_1(  nested, action, ...) LOOP_##nested(2, action, __VA_ARGS__)
#define LOOP_TYPE_SIZE_DIVIDE_5_2(  nested, action, ...) LOOP_##nested(2, action, __VA_ARGS__)
#define LOOP_TYPE_SIZE_DIVIDE_5_3(  nested, action, ...) LOOP_##nested(2, action, __VA_ARGS__)
#define LOOP_TYPE_SIZE_DIVIDE_5_4(  nested, action, ...) LOOP_##nested(1, action, __VA_ARGS__)
#define LOOP_TYPE_SIZE_DIVIDE_5_5(  nested, action, ...) LOOP_##nested(1, action, __VA_ARGS__)
#define LOOP_TYPE_SIZE_DIVIDE_5_6(  nested, action, ...) LOOP_##nested(0, action, __VA_ARGS__)
#define LOOP_TYPE_SIZE_DIVIDE_5_7(  nested, action, ...) LOOP_##nested(0, action, __VA_ARGS__)
#define LOOP_TYPE_SIZE_DIVIDE_5_8(  nested, action, ...) LOOP_##nested(0, action, __VA_ARGS__)
#define LOOP_TYPE_SIZE_DIVIDE_5_9(  nested, action, ...) LOOP_##nested(0, action, __VA_ARGS__)
#define LOOP_TYPE_SIZE_DIVIDE_5_10( nested, action, ...) LOOP_##nested(0, action, __VA_ARGS__)
#define LOOP_TYPE_SIZE_DIVIDE_5_11( nested, action, ...) LOOP_##nested(0, action, __VA_ARGS__)
#define LOOP_TYPE_SIZE_DIVIDE_6_0(  nested, action, ...) LOOP_##nested(4, action, __VA_ARGS__)
#define LOOP_TYPE_SIZE_DIVIDE_6_1(  nested, action, ...) LOOP_##nested(4, action, __VA_ARGS__)
#define LOOP_TYPE_SIZE_DIVIDE_6_2(  nested, action, ...) LOOP_##nested(4, action, __VA_ARGS__)
#define LOOP_TYPE_SIZE_DIVIDE_6_3(  nested, action, ...) LOOP_##nested(4, action, __VA_ARGS__)
#define LOOP_TYPE_SIZE_DIVIDE_6_4(  nested, action, ...) LOOP_##nested(2, action, __VA_ARGS__)
#define LOOP_TYPE_SIZE_DIVIDE_6_5(  nested, action, ...) LOOP_##nested(2, action, __VA_ARGS__)
#define LOOP_TYPE_SIZE_DIVIDE_6_6(  nested, action, ...) LOOP_##nested(1, action, __VA_ARGS__)
#define LOOP_TYPE_SIZE_DIVIDE_6_7(  nested, action, ...) LOOP_##nested(1, action, __VA_ARGS__)
#define LOOP_TYPE_SIZE_DIVIDE_6_8(  nested, action, ...) LOOP_##nested(0, action, __VA_ARGS__)
#define LOOP_TYPE_SIZE_DIVIDE_6_9(  nested, action, ...) LOOP_##nested(0, action, __VA_ARGS__)
#define LOOP_TYPE_SIZE_DIVIDE_6_10( nested, action, ...) LOOP_##nested(0, action, __VA_ARGS__)
#define LOOP_TYPE_SIZE_DIVIDE_6_11( nested, action, ...) LOOP_##nested(0, action, __VA_ARGS__)
#define LOOP_TYPE_SIZE_DIVIDE_7_0(  nested, action, ...) LOOP_##nested(4, action, __VA_ARGS__)
#define LOOP_TYPE_SIZE_DIVIDE_7_1(  nested, action, ...) LOOP_##nested(4, action, __VA_ARGS__)
#define LOOP_TYPE_SIZE_DIVIDE_7_2(  nested, action, ...) LOOP_##nested(4, action, __VA_ARGS__)
#define LOOP_TYPE_SIZE_DIVIDE_7_3(  nested, action, ...) LOOP_##nested(4, action, __VA_ARGS__)
#define LOOP_TYPE_SIZE_DIVIDE_7_4(  nested, action, ...) LOOP_##nested(2, action, __VA_ARGS__)
#define LOOP_TYPE_SIZE_DIVIDE_7_5(  nested, action, ...) LOOP_##nested(2, action, __VA_ARGS__)
#define LOOP_TYPE_SIZE_DIVIDE_7_6(  nested, action, ...) LOOP_##nested(1, action, __VA_ARGS__)
#define LOOP_TYPE_SIZE_DIVIDE_7_7(  nested, action, ...) LOOP_##nested(1, action, __VA_ARGS__)
#define LOOP_TYPE_SIZE_DIVIDE_7_8(  nested, action, ...) LOOP_##nested(0, action, __VA_ARGS__)
#define LOOP_TYPE_SIZE_DIVIDE_7_9(  nested, action, ...) LOOP_##nested(0, action, __VA_ARGS__)
#define LOOP_TYPE_SIZE_DIVIDE_7_10( nested, action, ...) LOOP_##nested(0, action, __VA_ARGS__)
#define LOOP_TYPE_SIZE_DIVIDE_7_11( nested, action, ...) LOOP_##nested(0, action, __VA_ARGS__)
#define LOOP_TYPE_SIZE_DIVIDE_8_0(  nested, action, ...) LOOP_##nested(8, action, __VA_ARGS__)
#define LOOP_TYPE_SIZE_DIVIDE_8_1(  nested, action, ...) LOOP_##nested(8, action, __VA_ARGS__)
#define LOOP_TYPE_SIZE_DIVIDE_8_2(  nested, action, ...) LOOP_##nested(8, action, __VA_ARGS__)
#define LOOP_TYPE_SIZE_DIVIDE_8_3(  nested, action, ...) LOOP_##nested(8, action, __VA_ARGS__)
#define LOOP_TYPE_SIZE_DIVIDE_8_4(  nested, action, ...) LOOP_##nested(4, action, __VA_ARGS__)
#define LOOP_TYPE_SIZE_DIVIDE_8_5(  nested, action, ...) LOOP_##nested(4, action, __VA_ARGS__)
#define LOOP_TYPE_SIZE_DIVIDE_8_6(  nested, action, ...) LOOP_##nested(2, action, __VA_ARGS__)
#define LOOP_TYPE_SIZE_DIVIDE_8_7(  nested, action, ...) LOOP_##nested(2, action, __VA_ARGS__)
#define LOOP_TYPE_SIZE_DIVIDE_8_8(  nested, action, ...) LOOP_##nested(1, action, __VA_ARGS__)
#define LOOP_TYPE_SIZE_DIVIDE_8_9(  nested, action, ...) LOOP_##nested(1, action, __VA_ARGS__)
#define LOOP_TYPE_SIZE_DIVIDE_8_10( nested, action, ...) LOOP_##nested(1, action, __VA_ARGS__)
#define LOOP_TYPE_SIZE_DIVIDE_8_11( nested, action, ...) LOOP_##nested(1, action, __VA_ARGS__)
#define LOOP_TYPE_SIZE_DIVIDE_9_0(  nested, action, ...) LOOP_##nested(8, action, __VA_ARGS__)
#define LOOP_TYPE_SIZE_DIVIDE_9_1(  nested, action, ...) LOOP_##nested(8, action, __VA_ARGS__)
#define LOOP_TYPE_SIZE_DIVIDE_9_2(  nested, action, ...) LOOP_##nested(8, action, __VA_ARGS__)
#define LOOP_TYPE_SIZE_DIVIDE_9_3(  nested, action, ...) LOOP_##nested(8, action, __VA_ARGS__)
#define LOOP_TYPE_SIZE_DIVIDE_9_4(  nested, action, ...) LOOP_##nested(4, action, __VA_ARGS__)
#define LOOP_TYPE_SIZE_DIVIDE_9_5(  nested, action, ...) LOOP_##nested(4, action, __VA_ARGS__)
#define LOOP_TYPE_SIZE_DIVIDE_9_6(  nested, action, ...) LOOP_##nested(2, action, __VA_ARGS__)
#define LOOP_TYPE_SIZE_DIVIDE_9_7(  nested, action, ...) LOOP_##nested(2, action, __VA_ARGS__)
#define LOOP_TYPE_SIZE_DIVIDE_9_8(  nested, action, ...) LOOP_##nested(1, action, __VA_ARGS__)
#define LOOP_TYPE_SIZE_DIVIDE_9_9(  nested, action, ...) LOOP_##nested(1, action, __VA_ARGS__)
#define LOOP_TYPE_SIZE_DIVIDE_9_10( nested, action, ...) LOOP_##nested(1, action, __VA_ARGS__)
#define LOOP_TYPE_SIZE_DIVIDE_9_11( nested, action, ...) LOOP_##nested(1, action, __VA_ARGS__)
#define LOOP_TYPE_SIZE_DIVIDE_10_0( nested, action, ...) LOOP_##nested(8, action, __VA_ARGS__)
#define LOOP_TYPE_SIZE_DIVIDE_10_1( nested, action, ...) LOOP_##nested(8, action, __VA_ARGS__)
#define LOOP_TYPE_SIZE_DIVIDE_10_2( nested, action, ...) LOOP_##nested(8, action, __VA_ARGS__)
#define LOOP_TYPE_SIZE_DIVIDE_10_3( nested, action, ...) LOOP_##nested(8, action, __VA_ARGS__)
#define LOOP_TYPE_SIZE_DIVIDE_10_4( nested, action, ...) LOOP_##nested(4, action, __VA_ARGS__)
#define LOOP_TYPE_SIZE_DIVIDE_10_5( nested, action, ...) LOOP_##nested(4, action, __VA_ARGS__)
#define LOOP_TYPE_SIZE_DIVIDE_10_6( nested, action, ...) LOOP_##nested(2, action, __VA_ARGS__)
#define LOOP_TYPE_SIZE_DIVIDE_10_7( nested, action, ...) LOOP_##nested(2, action, __VA_ARGS__)
#define LOOP_TYPE_SIZE_DIVIDE_10_8( nested, action, ...) LOOP_##nested(1, action, __VA_ARGS__)
#define LOOP_TYPE_SIZE_DIVIDE_10_9( nested, action, ...) LOOP_##nested(1, action, __VA_ARGS__)
#define LOOP_TYPE_SIZE_DIVIDE_10_10(nested, action, ...) LOOP_##nested(1, action, __VA_ARGS__)
#define LOOP_TYPE_SIZE_DIVIDE_10_11(nested, action, ...) LOOP_##nested(1, action, __VA_ARGS__)
#define LOOP_TYPE_SIZE_DIVIDE_11_0( nested, action, ...) LOOP_##nested(8, action, __VA_ARGS__)
#define LOOP_TYPE_SIZE_DIVIDE_11_1( nested, action, ...) LOOP_##nested(8, action, __VA_ARGS__)
#define LOOP_TYPE_SIZE_DIVIDE_11_2( nested, action, ...) LOOP_##nested(8, action, __VA_ARGS__)
#define LOOP_TYPE_SIZE_DIVIDE_11_3( nested, action, ...) LOOP_##nested(8, action, __VA_ARGS__)
#define LOOP_TYPE_SIZE_DIVIDE_11_4( nested, action, ...) LOOP_##nested(4, action, __VA_ARGS__)
#define LOOP_TYPE_SIZE_DIVIDE_11_5( nested, action, ...) LOOP_##nested(4, action, __VA_ARGS__)
#define LOOP_TYPE_SIZE_DIVIDE_11_6( nested, action, ...) LOOP_##nested(2, action, __VA_ARGS__)
#define LOOP_TYPE_SIZE_DIVIDE_11_7( nested, action, ...) LOOP_##nested(2, action, __VA_ARGS__)
#define LOOP_TYPE_SIZE_DIVIDE_11_8( nested, action, ...) LOOP_##nested(1, action, __VA_ARGS__)
#define LOOP_TYPE_SIZE_DIVIDE_11_9( nested, action, ...) LOOP_##nested(1, action, __VA_ARGS__)
#define LOOP_TYPE_SIZE_DIVIDE_11_10(nested, action, ...) LOOP_##nested(1, action, __VA_ARGS__)
#define LOOP_TYPE_SIZE_DIVIDE_11_11(nested, action, ...) LOOP_##nested(1, action, __VA_ARGS__)

#define IF_TYPE_SIZE_EQUAL_0_0(  action) action
#define IF_TYPE_SIZE_EQUAL_0_1(  action) action
#define IF_TYPE_SIZE_EQUAL_0_2(  action) action
#define IF_TYPE_SIZE_EQUAL_0_3(  action) action
#define IF_TYPE_SIZE_EQUAL_0_4(  action)
#define IF_TYPE_SIZE_EQUAL_0_5(  action)
#define IF_TYPE_SIZE_EQUAL_0_6(  action)
#define IF_TYPE_SIZE_EQUAL_0_7(  action)
#define IF_TYPE_SIZE_EQUAL_0_8(  action)
#define IF_TYPE_SIZE_EQUAL_0_9(  action)
#define IF_TYPE_SIZE_EQUAL_0_10( action)
#define IF_TYPE_SIZE_EQUAL_0_11( action)
#define IF_TYPE_SIZE_EQUAL_1_0(  action) action
#define IF_TYPE_SIZE_EQUAL_1_1(  action) action
#define IF_TYPE_SIZE_EQUAL_1_2(  action) action
#define IF_TYPE_SIZE_EQUAL_1_3(  action) action
#define IF_TYPE_SIZE_EQUAL_1_4(  action)
#define IF_TYPE_SIZE_EQUAL_1_5(  action)
#define IF_TYPE_SIZE_EQUAL_1_6(  action)
#define IF_TYPE_SIZE_EQUAL_1_7(  action)
#define IF_TYPE_SIZE_EQUAL_1_8(  action)
#define IF_TYPE_SIZE_EQUAL_1_9(  action)
#define IF_TYPE_SIZE_EQUAL_1_10( action)
#define IF_TYPE_SIZE_EQUAL_1_11( action)
#define IF_TYPE_SIZE_EQUAL_2_0(  action) action
#define IF_TYPE_SIZE_EQUAL_2_1(  action) action
#define IF_TYPE_SIZE_EQUAL_2_2(  action) action
#define IF_TYPE_SIZE_EQUAL_2_3(  action) action
#define IF_TYPE_SIZE_EQUAL_2_4(  action)
#define IF_TYPE_SIZE_EQUAL_2_5(  action)
#define IF_TYPE_SIZE_EQUAL_2_6(  action)
#define IF_TYPE_SIZE_EQUAL_2_7(  action)
#define IF_TYPE_SIZE_EQUAL_2_8(  action)
#define IF_TYPE_SIZE_EQUAL_2_9(  action)
#define IF_TYPE_SIZE_EQUAL_2_10( action)
#define IF_TYPE_SIZE_EQUAL_2_11( action)
#define IF_TYPE_SIZE_EQUAL_3_0(  action) action
#define IF_TYPE_SIZE_EQUAL_3_1(  action) action
#define IF_TYPE_SIZE_EQUAL_3_2(  action) action
#define IF_TYPE_SIZE_EQUAL_3_3(  action) action
#define IF_TYPE_SIZE_EQUAL_3_4(  action)
#define IF_TYPE_SIZE_EQUAL_3_5(  action)
#define IF_TYPE_SIZE_EQUAL_3_6(  action)
#define IF_TYPE_SIZE_EQUAL_3_7(  action)
#define IF_TYPE_SIZE_EQUAL_3_8(  action)
#define IF_TYPE_SIZE_EQUAL_3_9(  action)
#define IF_TYPE_SIZE_EQUAL_3_10( action)
#define IF_TYPE_SIZE_EQUAL_3_11( action)
#define IF_TYPE_SIZE_EQUAL_4_0(  action)
#define IF_TYPE_SIZE_EQUAL_4_1(  action)
#define IF_TYPE_SIZE_EQUAL_4_2(  action)
#define IF_TYPE_SIZE_EQUAL_4_3(  action)
#define IF_TYPE_SIZE_EQUAL_4_4(  action) action
#define IF_TYPE_SIZE_EQUAL_4_5(  action) action
#define IF_TYPE_SIZE_EQUAL_4_6(  action)
#define IF_TYPE_SIZE_EQUAL_4_7(  action)
#define IF_TYPE_SIZE_EQUAL_4_8(  action)
#define IF_TYPE_SIZE_EQUAL_4_9(  action)
#define IF_TYPE_SIZE_EQUAL_4_10( action)
#define IF_TYPE_SIZE_EQUAL_4_11( action)
#define IF_TYPE_SIZE_EQUAL_5_0(  action)
#define IF_TYPE_SIZE_EQUAL_5_1(  action)
#define IF_TYPE_SIZE_EQUAL_5_2(  action)
#define IF_TYPE_SIZE_EQUAL_5_3(  action)
#define IF_TYPE_SIZE_EQUAL_5_4(  action) action
#define IF_TYPE_SIZE_EQUAL_5_5(  action) action
#define IF_TYPE_SIZE_EQUAL_5_6(  action)
#define IF_TYPE_SIZE_EQUAL_5_7(  action)
#define IF_TYPE_SIZE_EQUAL_5_8(  action)
#define IF_TYPE_SIZE_EQUAL_5_9(  action)
#define IF_TYPE_SIZE_EQUAL_5_10( action)
#define IF_TYPE_SIZE_EQUAL_5_11( action)
#define IF_TYPE_SIZE_EQUAL_6_0(  action)
#define IF_TYPE_SIZE_EQUAL_6_1(  action)
#define IF_TYPE_SIZE_EQUAL_6_2(  action)
#define IF_TYPE_SIZE_EQUAL_6_3(  action)
#define IF_TYPE_SIZE_EQUAL_6_4(  action)
#define IF_TYPE_SIZE_EQUAL_6_5(  action)
#define IF_TYPE_SIZE_EQUAL_6_6(  action) action
#define IF_TYPE_SIZE_EQUAL_6_7(  action) action
#define IF_TYPE_SIZE_EQUAL_6_8(  action)
#define IF_TYPE_SIZE_EQUAL_6_9(  action)
#define IF_TYPE_SIZE_EQUAL_6_10( action)
#define IF_TYPE_SIZE_EQUAL_6_11( action)
#define IF_TYPE_SIZE_EQUAL_7_0(  action)
#define IF_TYPE_SIZE_EQUAL_7_1(  action)
#define IF_TYPE_SIZE_EQUAL_7_2(  action)
#define IF_TYPE_SIZE_EQUAL_7_3(  action)
#define IF_TYPE_SIZE_EQUAL_7_4(  action)
#define IF_TYPE_SIZE_EQUAL_7_5(  action)
#define IF_TYPE_SIZE_EQUAL_7_6(  action) action
#define IF_TYPE_SIZE_EQUAL_7_7(  action) action
#define IF_TYPE_SIZE_EQUAL_7_8(  action)
#define IF_TYPE_SIZE_EQUAL_7_9(  action)
#define IF_TYPE_SIZE_EQUAL_7_10( action)
#define IF_TYPE_SIZE_EQUAL_7_11( action)
#define IF_TYPE_SIZE_EQUAL_8_0(  action)
#define IF_TYPE_SIZE_EQUAL_8_1(  action)
#define IF_TYPE_SIZE_EQUAL_8_2(  action)
#define IF_TYPE_SIZE_EQUAL_8_3(  action)
#define IF_TYPE_SIZE_EQUAL_8_4(  action)
#define IF_TYPE_SIZE_EQUAL_8_5(  action)
#define IF_TYPE_SIZE_EQUAL_8_6(  action)
#define IF_TYPE_SIZE_EQUAL_8_7(  action)
#define IF_TYPE_SIZE_EQUAL_8_8(  action) action
#define IF_TYPE_SIZE_EQUAL_8_9(  action) action
#define IF_TYPE_SIZE_EQUAL_8_10( action) action
#define IF_TYPE_SIZE_EQUAL_8_11( action) action
#define IF_TYPE_SIZE_EQUAL_9_0(  action)
#define IF_TYPE_SIZE_EQUAL_9_1(  action)
#define IF_TYPE_SIZE_EQUAL_9_2(  action)
#define IF_TYPE_SIZE_EQUAL_9_3(  action)
#define IF_TYPE_SIZE_EQUAL_9_4(  action)
#define IF_TYPE_SIZE_EQUAL_9_5(  action)
#define IF_TYPE_SIZE_EQUAL_9_6(  action)
#define IF_TYPE_SIZE_EQUAL_9_7(  action)
#define IF_TYPE_SIZE_EQUAL_9_8(  action) action
#define IF_TYPE_SIZE_EQUAL_9_9(  action) action
#define IF_TYPE_SIZE_EQUAL_9_10( action) action
#define IF_TYPE_SIZE_EQUAL_9_11( action) action
#define IF_TYPE_SIZE_EQUAL_10_0( action)
#define IF_TYPE_SIZE_EQUAL_10_1( action)
#define IF_TYPE_SIZE_EQUAL_10_2( action)
#define IF_TYPE_SIZE_EQUAL_10_3( action)
#define IF_TYPE_SIZE_EQUAL_10_4( action)
#define IF_TYPE_SIZE_EQUAL_10_5( action)
#define IF_TYPE_SIZE_EQUAL_10_6( action)
#define IF_TYPE_SIZE_EQUAL_10_7( action)
#define IF_TYPE_SIZE_EQUAL_10_8( action) action
#define IF_TYPE_SIZE_EQUAL_10_9( action) action
#define IF_TYPE_SIZE_EQUAL_10_10(action) action
#define IF_TYPE_SIZE_EQUAL_10_11(action) action
#define IF_TYPE_SIZE_EQUAL_11_0( action)
#define IF_TYPE_SIZE_EQUAL_11_1( action)
#define IF_TYPE_SIZE_EQUAL_11_2( action)
#define IF_TYPE_SIZE_EQUAL_11_3( action)
#define IF_TYPE_SIZE_EQUAL_11_4( action)
#define IF_TYPE_SIZE_EQUAL_11_5( action)
#define IF_TYPE_SIZE_EQUAL_11_6( action)
#define IF_TYPE_SIZE_EQUAL_11_7( action)
#define IF_TYPE_SIZE_EQUAL_11_8( action) action
#define IF_TYPE_SIZE_EQUAL_11_9( action) action
#define IF_TYPE_SIZE_EQUAL_11_10(action) action
#define IF_TYPE_SIZE_EQUAL_11_11(action) action

#define IF_TYPE_SIZE_DIVIDABLE_0_0(  action) action
#define IF_TYPE_SIZE_DIVIDABLE_0_1(  action) action
#define IF_TYPE_SIZE_DIVIDABLE_0_2(  action) action
#define IF_TYPE_SIZE_DIVIDABLE_0_3(  action) action
#define IF_TYPE_SIZE_DIVIDABLE_0_4(  action)
#define IF_TYPE_SIZE_DIVIDABLE_0_5(  action)
#define IF_TYPE_SIZE_DIVIDABLE_0_6(  action)
#define IF_TYPE_SIZE_DIVIDABLE_0_7(  action)
#define IF_TYPE_SIZE_DIVIDABLE_0_8(  action)
#define IF_TYPE_SIZE_DIVIDABLE_0_9(  action)
#define IF_TYPE_SIZE_DIVIDABLE_0_10( action)
#define IF_TYPE_SIZE_DIVIDABLE_0_11( action)
#define IF_TYPE_SIZE_DIVIDABLE_1_0(  action) action
#define IF_TYPE_SIZE_DIVIDABLE_1_1(  action) action
#define IF_TYPE_SIZE_DIVIDABLE_1_2(  action) action
#define IF_TYPE_SIZE_DIVIDABLE_1_3(  action) action
#define IF_TYPE_SIZE_DIVIDABLE_1_4(  action)
#define IF_TYPE_SIZE_DIVIDABLE_1_5(  action)
#define IF_TYPE_SIZE_DIVIDABLE_1_6(  action)
#define IF_TYPE_SIZE_DIVIDABLE_1_7(  action)
#define IF_TYPE_SIZE_DIVIDABLE_1_8(  action)
#define IF_TYPE_SIZE_DIVIDABLE_1_9(  action)
#define IF_TYPE_SIZE_DIVIDABLE_1_10( action)
#define IF_TYPE_SIZE_DIVIDABLE_1_11( action)
#define IF_TYPE_SIZE_DIVIDABLE_2_0(  action) action
#define IF_TYPE_SIZE_DIVIDABLE_2_1(  action) action
#define IF_TYPE_SIZE_DIVIDABLE_2_2(  action) action
#define IF_TYPE_SIZE_DIVIDABLE_2_3(  action) action
#define IF_TYPE_SIZE_DIVIDABLE_2_4(  action)
#define IF_TYPE_SIZE_DIVIDABLE_2_5(  action)
#define IF_TYPE_SIZE_DIVIDABLE_2_6(  action)
#define IF_TYPE_SIZE_DIVIDABLE_2_7(  action)
#define IF_TYPE_SIZE_DIVIDABLE_2_8(  action)
#define IF_TYPE_SIZE_DIVIDABLE_2_9(  action)
#define IF_TYPE_SIZE_DIVIDABLE_2_10( action)
#define IF_TYPE_SIZE_DIVIDABLE_2_11( action)
#define IF_TYPE_SIZE_DIVIDABLE_3_0(  action) action
#define IF_TYPE_SIZE_DIVIDABLE_3_1(  action) action
#define IF_TYPE_SIZE_DIVIDABLE_3_2(  action) action
#define IF_TYPE_SIZE_DIVIDABLE_3_3(  action) action
#define IF_TYPE_SIZE_DIVIDABLE_3_4(  action)
#define IF_TYPE_SIZE_DIVIDABLE_3_5(  action)
#define IF_TYPE_SIZE_DIVIDABLE_3_6(  action)
#define IF_TYPE_SIZE_DIVIDABLE_3_7(  action)
#define IF_TYPE_SIZE_DIVIDABLE_3_8(  action)
#define IF_TYPE_SIZE_DIVIDABLE_3_9(  action)
#define IF_TYPE_SIZE_DIVIDABLE_3_10( action)
#define IF_TYPE_SIZE_DIVIDABLE_3_11( action)
#define IF_TYPE_SIZE_DIVIDABLE_4_0(  action) action
#define IF_TYPE_SIZE_DIVIDABLE_4_1(  action) action
#define IF_TYPE_SIZE_DIVIDABLE_4_2(  action) action
#define IF_TYPE_SIZE_DIVIDABLE_4_3(  action) action
#define IF_TYPE_SIZE_DIVIDABLE_4_4(  action) action
#define IF_TYPE_SIZE_DIVIDABLE_4_5(  action) action
#define IF_TYPE_SIZE_DIVIDABLE_4_6(  action)
#define IF_TYPE_SIZE_DIVIDABLE_4_7(  action)
#define IF_TYPE_SIZE_DIVIDABLE_4_8(  action)
#define IF_TYPE_SIZE_DIVIDABLE_4_9(  action)
#define IF_TYPE_SIZE_DIVIDABLE_4_10( action)
#define IF_TYPE_SIZE_DIVIDABLE_4_11( action)
#define IF_TYPE_SIZE_DIVIDABLE_5_0(  action) action
#define IF_TYPE_SIZE_DIVIDABLE_5_1(  action) action
#define IF_TYPE_SIZE_DIVIDABLE_5_2(  action) action
#define IF_TYPE_SIZE_DIVIDABLE_5_3(  action) action
#define IF_TYPE_SIZE_DIVIDABLE_5_4(  action) action
#define IF_TYPE_SIZE_DIVIDABLE_5_5(  action) action
#define IF_TYPE_SIZE_DIVIDABLE_5_6(  action)
#define IF_TYPE_SIZE_DIVIDABLE_5_7(  action)
#define IF_TYPE_SIZE_DIVIDABLE_5_8(  action)
#define IF_TYPE_SIZE_DIVIDABLE_5_9(  action)
#define IF_TYPE_SIZE_DIVIDABLE_5_10( action)
#define IF_TYPE_SIZE_DIVIDABLE_5_11( action)
#define IF_TYPE_SIZE_DIVIDABLE_6_0(  action) action
#define IF_TYPE_SIZE_DIVIDABLE_6_1(  action) action
#define IF_TYPE_SIZE_DIVIDABLE_6_2(  action) action
#define IF_TYPE_SIZE_DIVIDABLE_6_3(  action) action
#define IF_TYPE_SIZE_DIVIDABLE_6_4(  action) action
#define IF_TYPE_SIZE_DIVIDABLE_6_5(  action) action
#define IF_TYPE_SIZE_DIVIDABLE_6_6(  action) action
#define IF_TYPE_SIZE_DIVIDABLE_6_7(  action) action
#define IF_TYPE_SIZE_DIVIDABLE_6_8(  action)
#define IF_TYPE_SIZE_DIVIDABLE_6_9(  action)
#define IF_TYPE_SIZE_DIVIDABLE_6_10( action)
#define IF_TYPE_SIZE_DIVIDABLE_6_11( action)
#define IF_TYPE_SIZE_DIVIDABLE_7_0(  action) action
#define IF_TYPE_SIZE_DIVIDABLE_7_1(  action) action
#define IF_TYPE_SIZE_DIVIDABLE_7_2(  action) action
#define IF_TYPE_SIZE_DIVIDABLE_7_3(  action) action
#define IF_TYPE_SIZE_DIVIDABLE_7_4(  action) action
#define IF_TYPE_SIZE_DIVIDABLE_7_5(  action) action
#define IF_TYPE_SIZE_DIVIDABLE_7_6(  action) action
#define IF_TYPE_SIZE_DIVIDABLE_7_7(  action) action
#define IF_TYPE_SIZE_DIVIDABLE_7_8(  action)
#define IF_TYPE_SIZE_DIVIDABLE_7_9(  action)
#define IF_TYPE_SIZE_DIVIDABLE_7_10( action)
#define IF_TYPE_SIZE_DIVIDABLE_7_11( action)
#define IF_TYPE_SIZE_DIVIDABLE_8_0(  action) action
#define IF_TYPE_SIZE_DIVIDABLE_8_1(  action) action
#define IF_TYPE_SIZE_DIVIDABLE_8_2(  action) action
#define IF_TYPE_SIZE_DIVIDABLE_8_3(  action) action
#define IF_TYPE_SIZE_DIVIDABLE_8_4(  action) action
#define IF_TYPE_SIZE_DIVIDABLE_8_5(  action) action
#define IF_TYPE_SIZE_DIVIDABLE_8_6(  action) action
#define IF_TYPE_SIZE_DIVIDABLE_8_7(  action) action
#define IF_TYPE_SIZE_DIVIDABLE_8_8(  action) action
#define IF_TYPE_SIZE_DIVIDABLE_8_9(  action) action
#define IF_TYPE_SIZE_DIVIDABLE_8_10( action) action
#define IF_TYPE_SIZE_DIVIDABLE_8_11( action) action
#define IF_TYPE_SIZE_DIVIDABLE_9_0(  action) action
#define IF_TYPE_SIZE_DIVIDABLE_9_1(  action) action
#define IF_TYPE_SIZE_DIVIDABLE_9_2(  action) action
#define IF_TYPE_SIZE_DIVIDABLE_9_3(  action) action
#define IF_TYPE_SIZE_DIVIDABLE_9_4(  action) action
#define IF_TYPE_SIZE_DIVIDABLE_9_5(  action) action
#define IF_TYPE_SIZE_DIVIDABLE_9_6(  action) action
#define IF_TYPE_SIZE_DIVIDABLE_9_7(  action) action
#define IF_TYPE_SIZE_DIVIDABLE_9_8(  action) action
#define IF_TYPE_SIZE_DIVIDABLE_9_9(  action) action
#define IF_TYPE_SIZE_DIVIDABLE_9_10( action) action
#define IF_TYPE_SIZE_DIVIDABLE_9_11( action) action
#define IF_TYPE_SIZE_DIVIDABLE_10_0( action) action
#define IF_TYPE_SIZE_DIVIDABLE_10_1( action) action
#define IF_TYPE_SIZE_DIVIDABLE_10_2( action) action
#define IF_TYPE_SIZE_DIVIDABLE_10_3( action) action
#define IF_TYPE_SIZE_DIVIDABLE_10_4( action) action
#define IF_TYPE_SIZE_DIVIDABLE_10_5( action) action
#define IF_TYPE_SIZE_DIVIDABLE_10_6( action) action
#define IF_TYPE_SIZE_DIVIDABLE_10_7( action) action
#define IF_TYPE_SIZE_DIVIDABLE_10_8( action) action
#define IF_TYPE_SIZE_DIVIDABLE_10_9( action) action
#define IF_TYPE_SIZE_DIVIDABLE_10_10(action) action
#define IF_TYPE_SIZE_DIVIDABLE_10_11(action) action
#define IF_TYPE_SIZE_DIVIDABLE_11_0( action) action
#define IF_TYPE_SIZE_DIVIDABLE_11_1( action) action
#define IF_TYPE_SIZE_DIVIDABLE_11_2( action) action
#define IF_TYPE_SIZE_DIVIDABLE_11_3( action) action
#define IF_TYPE_SIZE_DIVIDABLE_11_4( action) action
#define IF_TYPE_SIZE_DIVIDABLE_11_5( action) action
#define IF_TYPE_SIZE_DIVIDABLE_11_6( action) action
#define IF_TYPE_SIZE_DIVIDABLE_11_7( action) action
#define IF_TYPE_SIZE_DIVIDABLE_11_8( action) action
#define IF_TYPE_SIZE_DIVIDABLE_11_9( action) action
#define IF_TYPE_SIZE_DIVIDABLE_11_10(action) action
#define IF_TYPE_SIZE_DIVIDABLE_11_11(action) action

#define SINGLE_ASSERT_TYPE(T, U)\
    static_assert(T == U ? true : !std::same_as<TYPE(T), TYPE(U)>);
#define        ASSERT_TYPE(T) LOOP_TYPES_1(SINGLE_ASSERT_TYPE, T);
LOOP_TYPES_0(  ASSERT_TYPE);
#undef  SINGLE_ASSERT_TYPE
#undef         ASSERT_TYPE

// NOLINTNEXTLINE [cppcoreguidelines-macro-usage]
#define SINGLE_ASSERT_TYPE_SIZE_DIVIDE(T, U)\
    static_assert(TYPE_SIZE_DIVIDE(T, U) == sizeof(TYPE(T)) / sizeof(TYPE(U)));
// NOLINTNEXTLINE [cppcoreguidelines-macro-usage]
#define        ASSERT_TYPE_SIZE_DIVIDE(T) LOOP_TYPES_1(SINGLE_ASSERT_TYPE_SIZE_DIVIDE, T);
LOOP_TYPES_0(  ASSERT_TYPE_SIZE_DIVIDE);
#undef  SINGLE_ASSERT_TYPE_SIZE_DIVIDE
#undef         ASSERT_TYPE_SIZE_DIVIDE

// NOLINTNEXTLINE [cppcoreguidelines-macro-usage]
#define SINGLE_ASSERT_LOOP_TYPE_SIZE_DIVIDE_FUNCTION(index) " "
// NOLINTNEXTLINE [cppcoreguidelines-macro-usage]
#define SINGLE_ASSERT_LOOP_TYPE_SIZE_DIVIDE(T, U)\
    static_assert(sizeof "" LOOP_TYPE_SIZE_DIVIDE_2(T, U, SINGLE_ASSERT_LOOP_TYPE_SIZE_DIVIDE_FUNCTION) == sizeof(TYPE(T)) / sizeof(TYPE(U)) + 1);
// NOLINTNEXTLINE [cppcoreguidelines-macro-usage]
#define        ASSERT_LOOP_TYPE_SIZE_DIVIDE(T) LOOP_TYPES_1(SINGLE_ASSERT_LOOP_TYPE_SIZE_DIVIDE, T);
LOOP_TYPES_0(  ASSERT_LOOP_TYPE_SIZE_DIVIDE);
#undef  SINGLE_ASSERT_LOOP_TYPE_SIZE_DIVIDE_FUNCTION
#undef  SINGLE_ASSERT_LOOP_TYPE_SIZE_DIVIDE
#undef         ASSERT_LOOP_TYPE_SIZE_DIVIDE

// NOLINTNEXTLINE [cppcoreguidelines-macro-usage]
#define SINGLE_ASSERT_TYPE_SIZE_EQUAL(T, U)\
    static_assert(sizeof(TYPE(T)) == sizeof(TYPE(U)) ? IF_TYPE_SIZE_EQUAL(T, U, 1) + 0 : IF_TYPE_SIZE_EQUAL(T, U, 1) - 1);
// NOLINTNEXTLINE [cppcoreguidelines-macro-usage]
#define        ASSERT_TYPE_SIZE_EQUAL(T) LOOP_TYPES_1(SINGLE_ASSERT_TYPE_SIZE_EQUAL, T);
LOOP_TYPES_0(  ASSERT_TYPE_SIZE_EQUAL);
#undef  SINGLE_ASSERT_TYPE_SIZE_EQUAL
#undef         ASSERT_TYPE_SIZE_EQUAL

// NOLINTNEXTLINE [cppcoreguidelines-macro-usage]
#define SINGLE_ASSERT_TYPE_SIZE_DIVIDABLE(T, U)\
    static_assert(sizeof(TYPE(T)) / sizeof(TYPE(U)) != 0 ? IF_TYPE_SIZE_DIVIDABLE(T, U, 1) + 0 : IF_TYPE_SIZE_DIVIDABLE(T, U, 1) - 1);
// NOLINTNEXTLINE [cppcoreguidelines-macro-usage]
#define        ASSERT_TYPE_SIZE_DIVIDABLE(T) LOOP_TYPES_1(SINGLE_ASSERT_TYPE_SIZE_DIVIDABLE, T);
LOOP_TYPES_0(  ASSERT_TYPE_SIZE_DIVIDABLE);
#undef  SINGLE_ASSERT_TYPE_SIZE_DIVIDABLE
#undef         ASSERT_TYPE_SIZE_DIVIDABLE

// clang-format on
