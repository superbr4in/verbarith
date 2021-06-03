#pragma once

// clang-format off

#define LOOP_0(count, action, ...) LOOP_##count##_0(action, __VA_ARGS__)
#define LOOP_1(count, action, ...) LOOP_##count##_1(action, __VA_ARGS__)
#define LOOP_2(count, action, ...) LOOP_##count##_2(action, __VA_ARGS__)

#define IF_NOT_EQUAL(value_1, value_2, action) IF_NOT_EQUAL_##value_1##_##value_2(action)

#define LOOP_0_0( action, ...)
#define LOOP_1_0( action, ...)                                action(__VA_ARGS__ __VA_OPT__(,)  0)
#define LOOP_2_0( action, ...) LOOP_1_0( action, __VA_ARGS__) action(__VA_ARGS__ __VA_OPT__(,)  1)
#define LOOP_3_0( action, ...) LOOP_2_0( action, __VA_ARGS__) action(__VA_ARGS__ __VA_OPT__(,)  2)
#define LOOP_4_0( action, ...) LOOP_3_0( action, __VA_ARGS__) action(__VA_ARGS__ __VA_OPT__(,)  3)
#define LOOP_5_0( action, ...) LOOP_4_0( action, __VA_ARGS__) action(__VA_ARGS__ __VA_OPT__(,)  4)
#define LOOP_6_0( action, ...) LOOP_5_0( action, __VA_ARGS__) action(__VA_ARGS__ __VA_OPT__(,)  5)
#define LOOP_7_0( action, ...) LOOP_6_0( action, __VA_ARGS__) action(__VA_ARGS__ __VA_OPT__(,)  6)
#define LOOP_8_0( action, ...) LOOP_7_0( action, __VA_ARGS__) action(__VA_ARGS__ __VA_OPT__(,)  7)
#define LOOP_9_0( action, ...) LOOP_8_0( action, __VA_ARGS__) action(__VA_ARGS__ __VA_OPT__(,)  8)
#define LOOP_10_0(action, ...) LOOP_9_0( action, __VA_ARGS__) action(__VA_ARGS__ __VA_OPT__(,)  9)
#define LOOP_11_0(action, ...) LOOP_10_0(action, __VA_ARGS__) action(__VA_ARGS__ __VA_OPT__(,) 10)
#define LOOP_12_0(action, ...) LOOP_11_0(action, __VA_ARGS__) action(__VA_ARGS__ __VA_OPT__(,) 11)
#define LOOP_13_0(action, ...) LOOP_12_0(action, __VA_ARGS__) action(__VA_ARGS__ __VA_OPT__(,) 12)
#define LOOP_14_0(action, ...) LOOP_13_0(action, __VA_ARGS__) action(__VA_ARGS__ __VA_OPT__(,) 13)
#define LOOP_15_0(action, ...) LOOP_14_0(action, __VA_ARGS__) action(__VA_ARGS__ __VA_OPT__(,) 14)
#define LOOP_16_0(action, ...) LOOP_15_0(action, __VA_ARGS__) action(__VA_ARGS__ __VA_OPT__(,) 15)
#define LOOP_17_0(action, ...) LOOP_16_0(action, __VA_ARGS__) action(__VA_ARGS__ __VA_OPT__(,) 16)
#define LOOP_18_0(action, ...) LOOP_17_0(action, __VA_ARGS__) action(__VA_ARGS__ __VA_OPT__(,) 17)
#define LOOP_19_0(action, ...) LOOP_18_0(action, __VA_ARGS__) action(__VA_ARGS__ __VA_OPT__(,) 18)
#define LOOP_20_0(action, ...) LOOP_19_0(action, __VA_ARGS__) action(__VA_ARGS__ __VA_OPT__(,) 19)
#define LOOP_21_0(action, ...) LOOP_20_0(action, __VA_ARGS__) action(__VA_ARGS__ __VA_OPT__(,) 20)
#define LOOP_22_0(action, ...) LOOP_21_0(action, __VA_ARGS__) action(__VA_ARGS__ __VA_OPT__(,) 21)
#define LOOP_23_0(action, ...) LOOP_22_0(action, __VA_ARGS__) action(__VA_ARGS__ __VA_OPT__(,) 22)
#define LOOP_24_0(action, ...) LOOP_23_0(action, __VA_ARGS__) action(__VA_ARGS__ __VA_OPT__(,) 23)
#define LOOP_25_0(action, ...) LOOP_24_0(action, __VA_ARGS__) action(__VA_ARGS__ __VA_OPT__(,) 24)
#define LOOP_26_0(action, ...) LOOP_25_0(action, __VA_ARGS__) action(__VA_ARGS__ __VA_OPT__(,) 25)
#define LOOP_27_0(action, ...) LOOP_26_0(action, __VA_ARGS__) action(__VA_ARGS__ __VA_OPT__(,) 26)
#define LOOP_28_0(action, ...) LOOP_27_0(action, __VA_ARGS__) action(__VA_ARGS__ __VA_OPT__(,) 27)
#define LOOP_29_0(action, ...) LOOP_28_0(action, __VA_ARGS__) action(__VA_ARGS__ __VA_OPT__(,) 28)
#define LOOP_30_0(action, ...) LOOP_29_0(action, __VA_ARGS__) action(__VA_ARGS__ __VA_OPT__(,) 29)
#define LOOP_31_0(action, ...) LOOP_30_0(action, __VA_ARGS__) action(__VA_ARGS__ __VA_OPT__(,) 30)
#define LOOP_32_0(action, ...) LOOP_31_0(action, __VA_ARGS__) action(__VA_ARGS__ __VA_OPT__(,) 31)
#define LOOP_33_0(action, ...) LOOP_32_0(action, __VA_ARGS__) action(__VA_ARGS__ __VA_OPT__(,) 32)
#define LOOP_34_0(action, ...) LOOP_33_0(action, __VA_ARGS__) action(__VA_ARGS__ __VA_OPT__(,) 33)
#define LOOP_35_0(action, ...) LOOP_34_0(action, __VA_ARGS__) action(__VA_ARGS__ __VA_OPT__(,) 34)
#define LOOP_36_0(action, ...) LOOP_35_0(action, __VA_ARGS__) action(__VA_ARGS__ __VA_OPT__(,) 35)
#define LOOP_37_0(action, ...) LOOP_36_0(action, __VA_ARGS__) action(__VA_ARGS__ __VA_OPT__(,) 36)
#define LOOP_38_0(action, ...) LOOP_37_0(action, __VA_ARGS__) action(__VA_ARGS__ __VA_OPT__(,) 37)
#define LOOP_39_0(action, ...) LOOP_38_0(action, __VA_ARGS__) action(__VA_ARGS__ __VA_OPT__(,) 38)
#define LOOP_40_0(action, ...) LOOP_39_0(action, __VA_ARGS__) action(__VA_ARGS__ __VA_OPT__(,) 39)
#define LOOP_41_0(action, ...) LOOP_40_0(action, __VA_ARGS__) action(__VA_ARGS__ __VA_OPT__(,) 40)
#define LOOP_42_0(action, ...) LOOP_41_0(action, __VA_ARGS__) action(__VA_ARGS__ __VA_OPT__(,) 41)
#define LOOP_43_0(action, ...) LOOP_42_0(action, __VA_ARGS__) action(__VA_ARGS__ __VA_OPT__(,) 42)
#define LOOP_44_0(action, ...) LOOP_43_0(action, __VA_ARGS__) action(__VA_ARGS__ __VA_OPT__(,) 43)
#define LOOP_45_0(action, ...) LOOP_44_0(action, __VA_ARGS__) action(__VA_ARGS__ __VA_OPT__(,) 44)
#define LOOP_46_0(action, ...) LOOP_45_0(action, __VA_ARGS__) action(__VA_ARGS__ __VA_OPT__(,) 45)
#define LOOP_47_0(action, ...) LOOP_46_0(action, __VA_ARGS__) action(__VA_ARGS__ __VA_OPT__(,) 46)
#define LOOP_48_0(action, ...) LOOP_47_0(action, __VA_ARGS__) action(__VA_ARGS__ __VA_OPT__(,) 47)
#define LOOP_49_0(action, ...) LOOP_48_0(action, __VA_ARGS__) action(__VA_ARGS__ __VA_OPT__(,) 48)
#define LOOP_50_0(action, ...) LOOP_49_0(action, __VA_ARGS__) action(__VA_ARGS__ __VA_OPT__(,) 49)
#define LOOP_51_0(action, ...) LOOP_50_0(action, __VA_ARGS__) action(__VA_ARGS__ __VA_OPT__(,) 50)
#define LOOP_52_0(action, ...) LOOP_51_0(action, __VA_ARGS__) action(__VA_ARGS__ __VA_OPT__(,) 51)
#define LOOP_53_0(action, ...) LOOP_52_0(action, __VA_ARGS__) action(__VA_ARGS__ __VA_OPT__(,) 52)
#define LOOP_54_0(action, ...) LOOP_53_0(action, __VA_ARGS__) action(__VA_ARGS__ __VA_OPT__(,) 53)
#define LOOP_55_0(action, ...) LOOP_54_0(action, __VA_ARGS__) action(__VA_ARGS__ __VA_OPT__(,) 54)
#define LOOP_56_0(action, ...) LOOP_55_0(action, __VA_ARGS__) action(__VA_ARGS__ __VA_OPT__(,) 55)
#define LOOP_57_0(action, ...) LOOP_56_0(action, __VA_ARGS__) action(__VA_ARGS__ __VA_OPT__(,) 56)
#define LOOP_58_0(action, ...) LOOP_57_0(action, __VA_ARGS__) action(__VA_ARGS__ __VA_OPT__(,) 57)
#define LOOP_59_0(action, ...) LOOP_58_0(action, __VA_ARGS__) action(__VA_ARGS__ __VA_OPT__(,) 58)
#define LOOP_60_0(action, ...) LOOP_59_0(action, __VA_ARGS__) action(__VA_ARGS__ __VA_OPT__(,) 59)
#define LOOP_61_0(action, ...) LOOP_60_0(action, __VA_ARGS__) action(__VA_ARGS__ __VA_OPT__(,) 60)
#define LOOP_62_0(action, ...) LOOP_61_0(action, __VA_ARGS__) action(__VA_ARGS__ __VA_OPT__(,) 61)
#define LOOP_63_0(action, ...) LOOP_62_0(action, __VA_ARGS__) action(__VA_ARGS__ __VA_OPT__(,) 62)
#define LOOP_64_0(action, ...) LOOP_63_0(action, __VA_ARGS__) action(__VA_ARGS__ __VA_OPT__(,) 63)

#define LOOP_0_1( action, ...)
#define LOOP_1_1( action, ...)                                action(__VA_ARGS__ __VA_OPT__(,)  0)
#define LOOP_2_1( action, ...) LOOP_1_1( action, __VA_ARGS__) action(__VA_ARGS__ __VA_OPT__(,)  1)
#define LOOP_3_1( action, ...) LOOP_2_1( action, __VA_ARGS__) action(__VA_ARGS__ __VA_OPT__(,)  2)
#define LOOP_4_1( action, ...) LOOP_3_1( action, __VA_ARGS__) action(__VA_ARGS__ __VA_OPT__(,)  3)
#define LOOP_5_1( action, ...) LOOP_4_1( action, __VA_ARGS__) action(__VA_ARGS__ __VA_OPT__(,)  4)
#define LOOP_6_1( action, ...) LOOP_5_1( action, __VA_ARGS__) action(__VA_ARGS__ __VA_OPT__(,)  5)
#define LOOP_7_1( action, ...) LOOP_6_1( action, __VA_ARGS__) action(__VA_ARGS__ __VA_OPT__(,)  6)
#define LOOP_8_1( action, ...) LOOP_7_1( action, __VA_ARGS__) action(__VA_ARGS__ __VA_OPT__(,)  7)
#define LOOP_9_1( action, ...) LOOP_8_1( action, __VA_ARGS__) action(__VA_ARGS__ __VA_OPT__(,)  8)
#define LOOP_10_1(action, ...) LOOP_9_1( action, __VA_ARGS__) action(__VA_ARGS__ __VA_OPT__(,)  9)
#define LOOP_11_1(action, ...) LOOP_10_1(action, __VA_ARGS__) action(__VA_ARGS__ __VA_OPT__(,) 10)
#define LOOP_12_1(action, ...) LOOP_11_1(action, __VA_ARGS__) action(__VA_ARGS__ __VA_OPT__(,) 11)
#define LOOP_13_1(action, ...) LOOP_12_1(action, __VA_ARGS__) action(__VA_ARGS__ __VA_OPT__(,) 12)
#define LOOP_14_1(action, ...) LOOP_13_1(action, __VA_ARGS__) action(__VA_ARGS__ __VA_OPT__(,) 13)
#define LOOP_15_1(action, ...) LOOP_14_1(action, __VA_ARGS__) action(__VA_ARGS__ __VA_OPT__(,) 14)
#define LOOP_16_1(action, ...) LOOP_15_1(action, __VA_ARGS__) action(__VA_ARGS__ __VA_OPT__(,) 15)
#define LOOP_17_1(action, ...) LOOP_16_1(action, __VA_ARGS__) action(__VA_ARGS__ __VA_OPT__(,) 16)
#define LOOP_18_1(action, ...) LOOP_17_1(action, __VA_ARGS__) action(__VA_ARGS__ __VA_OPT__(,) 17)
#define LOOP_19_1(action, ...) LOOP_18_1(action, __VA_ARGS__) action(__VA_ARGS__ __VA_OPT__(,) 18)
#define LOOP_20_1(action, ...) LOOP_19_1(action, __VA_ARGS__) action(__VA_ARGS__ __VA_OPT__(,) 19)
#define LOOP_21_1(action, ...) LOOP_20_1(action, __VA_ARGS__) action(__VA_ARGS__ __VA_OPT__(,) 20)
#define LOOP_22_1(action, ...) LOOP_21_1(action, __VA_ARGS__) action(__VA_ARGS__ __VA_OPT__(,) 21)
#define LOOP_23_1(action, ...) LOOP_22_1(action, __VA_ARGS__) action(__VA_ARGS__ __VA_OPT__(,) 22)
#define LOOP_24_1(action, ...) LOOP_23_1(action, __VA_ARGS__) action(__VA_ARGS__ __VA_OPT__(,) 23)
#define LOOP_25_1(action, ...) LOOP_24_1(action, __VA_ARGS__) action(__VA_ARGS__ __VA_OPT__(,) 24)
#define LOOP_26_1(action, ...) LOOP_25_1(action, __VA_ARGS__) action(__VA_ARGS__ __VA_OPT__(,) 25)
#define LOOP_27_1(action, ...) LOOP_26_1(action, __VA_ARGS__) action(__VA_ARGS__ __VA_OPT__(,) 26)
#define LOOP_28_1(action, ...) LOOP_27_1(action, __VA_ARGS__) action(__VA_ARGS__ __VA_OPT__(,) 27)
#define LOOP_29_1(action, ...) LOOP_28_1(action, __VA_ARGS__) action(__VA_ARGS__ __VA_OPT__(,) 28)
#define LOOP_30_1(action, ...) LOOP_29_1(action, __VA_ARGS__) action(__VA_ARGS__ __VA_OPT__(,) 29)
#define LOOP_31_1(action, ...) LOOP_30_1(action, __VA_ARGS__) action(__VA_ARGS__ __VA_OPT__(,) 30)
#define LOOP_32_1(action, ...) LOOP_31_1(action, __VA_ARGS__) action(__VA_ARGS__ __VA_OPT__(,) 31)
#define LOOP_33_1(action, ...) LOOP_32_1(action, __VA_ARGS__) action(__VA_ARGS__ __VA_OPT__(,) 32)
#define LOOP_34_1(action, ...) LOOP_33_1(action, __VA_ARGS__) action(__VA_ARGS__ __VA_OPT__(,) 33)
#define LOOP_35_1(action, ...) LOOP_34_1(action, __VA_ARGS__) action(__VA_ARGS__ __VA_OPT__(,) 34)
#define LOOP_36_1(action, ...) LOOP_35_1(action, __VA_ARGS__) action(__VA_ARGS__ __VA_OPT__(,) 35)
#define LOOP_37_1(action, ...) LOOP_36_1(action, __VA_ARGS__) action(__VA_ARGS__ __VA_OPT__(,) 36)
#define LOOP_38_1(action, ...) LOOP_37_1(action, __VA_ARGS__) action(__VA_ARGS__ __VA_OPT__(,) 37)
#define LOOP_39_1(action, ...) LOOP_38_1(action, __VA_ARGS__) action(__VA_ARGS__ __VA_OPT__(,) 38)
#define LOOP_40_1(action, ...) LOOP_39_1(action, __VA_ARGS__) action(__VA_ARGS__ __VA_OPT__(,) 39)
#define LOOP_41_1(action, ...) LOOP_40_1(action, __VA_ARGS__) action(__VA_ARGS__ __VA_OPT__(,) 40)
#define LOOP_42_1(action, ...) LOOP_41_1(action, __VA_ARGS__) action(__VA_ARGS__ __VA_OPT__(,) 41)
#define LOOP_43_1(action, ...) LOOP_42_1(action, __VA_ARGS__) action(__VA_ARGS__ __VA_OPT__(,) 42)
#define LOOP_44_1(action, ...) LOOP_43_1(action, __VA_ARGS__) action(__VA_ARGS__ __VA_OPT__(,) 43)
#define LOOP_45_1(action, ...) LOOP_44_1(action, __VA_ARGS__) action(__VA_ARGS__ __VA_OPT__(,) 44)
#define LOOP_46_1(action, ...) LOOP_45_1(action, __VA_ARGS__) action(__VA_ARGS__ __VA_OPT__(,) 45)
#define LOOP_47_1(action, ...) LOOP_46_1(action, __VA_ARGS__) action(__VA_ARGS__ __VA_OPT__(,) 46)
#define LOOP_48_1(action, ...) LOOP_47_1(action, __VA_ARGS__) action(__VA_ARGS__ __VA_OPT__(,) 47)
#define LOOP_49_1(action, ...) LOOP_48_1(action, __VA_ARGS__) action(__VA_ARGS__ __VA_OPT__(,) 48)
#define LOOP_50_1(action, ...) LOOP_49_1(action, __VA_ARGS__) action(__VA_ARGS__ __VA_OPT__(,) 49)
#define LOOP_51_1(action, ...) LOOP_50_1(action, __VA_ARGS__) action(__VA_ARGS__ __VA_OPT__(,) 50)
#define LOOP_52_1(action, ...) LOOP_51_1(action, __VA_ARGS__) action(__VA_ARGS__ __VA_OPT__(,) 51)
#define LOOP_53_1(action, ...) LOOP_52_1(action, __VA_ARGS__) action(__VA_ARGS__ __VA_OPT__(,) 52)
#define LOOP_54_1(action, ...) LOOP_53_1(action, __VA_ARGS__) action(__VA_ARGS__ __VA_OPT__(,) 53)
#define LOOP_55_1(action, ...) LOOP_54_1(action, __VA_ARGS__) action(__VA_ARGS__ __VA_OPT__(,) 54)
#define LOOP_56_1(action, ...) LOOP_55_1(action, __VA_ARGS__) action(__VA_ARGS__ __VA_OPT__(,) 55)
#define LOOP_57_1(action, ...) LOOP_56_1(action, __VA_ARGS__) action(__VA_ARGS__ __VA_OPT__(,) 56)
#define LOOP_58_1(action, ...) LOOP_57_1(action, __VA_ARGS__) action(__VA_ARGS__ __VA_OPT__(,) 57)
#define LOOP_59_1(action, ...) LOOP_58_1(action, __VA_ARGS__) action(__VA_ARGS__ __VA_OPT__(,) 58)
#define LOOP_60_1(action, ...) LOOP_59_1(action, __VA_ARGS__) action(__VA_ARGS__ __VA_OPT__(,) 59)
#define LOOP_61_1(action, ...) LOOP_60_1(action, __VA_ARGS__) action(__VA_ARGS__ __VA_OPT__(,) 60)
#define LOOP_62_1(action, ...) LOOP_61_1(action, __VA_ARGS__) action(__VA_ARGS__ __VA_OPT__(,) 61)
#define LOOP_63_1(action, ...) LOOP_62_1(action, __VA_ARGS__) action(__VA_ARGS__ __VA_OPT__(,) 62)
#define LOOP_64_1(action, ...) LOOP_63_1(action, __VA_ARGS__) action(__VA_ARGS__ __VA_OPT__(,) 63)

#define LOOP_0_2( action, ...)
#define LOOP_1_2( action, ...)                                action(__VA_ARGS__ __VA_OPT__(,)  0)
#define LOOP_2_2( action, ...) LOOP_1_2( action, __VA_ARGS__) action(__VA_ARGS__ __VA_OPT__(,)  1)
#define LOOP_3_2( action, ...) LOOP_2_2( action, __VA_ARGS__) action(__VA_ARGS__ __VA_OPT__(,)  2)
#define LOOP_4_2( action, ...) LOOP_3_2( action, __VA_ARGS__) action(__VA_ARGS__ __VA_OPT__(,)  3)
#define LOOP_5_2( action, ...) LOOP_4_2( action, __VA_ARGS__) action(__VA_ARGS__ __VA_OPT__(,)  4)
#define LOOP_6_2( action, ...) LOOP_5_2( action, __VA_ARGS__) action(__VA_ARGS__ __VA_OPT__(,)  5)
#define LOOP_7_2( action, ...) LOOP_6_2( action, __VA_ARGS__) action(__VA_ARGS__ __VA_OPT__(,)  6)
#define LOOP_8_2( action, ...) LOOP_7_2( action, __VA_ARGS__) action(__VA_ARGS__ __VA_OPT__(,)  7)
#define LOOP_9_2( action, ...) LOOP_8_2( action, __VA_ARGS__) action(__VA_ARGS__ __VA_OPT__(,)  8)
#define LOOP_10_2(action, ...) LOOP_9_2( action, __VA_ARGS__) action(__VA_ARGS__ __VA_OPT__(,)  9)
#define LOOP_11_2(action, ...) LOOP_10_2(action, __VA_ARGS__) action(__VA_ARGS__ __VA_OPT__(,) 10)
#define LOOP_12_2(action, ...) LOOP_11_2(action, __VA_ARGS__) action(__VA_ARGS__ __VA_OPT__(,) 11)
#define LOOP_13_2(action, ...) LOOP_12_2(action, __VA_ARGS__) action(__VA_ARGS__ __VA_OPT__(,) 12)
#define LOOP_14_2(action, ...) LOOP_13_2(action, __VA_ARGS__) action(__VA_ARGS__ __VA_OPT__(,) 13)
#define LOOP_15_2(action, ...) LOOP_14_2(action, __VA_ARGS__) action(__VA_ARGS__ __VA_OPT__(,) 14)
#define LOOP_16_2(action, ...) LOOP_15_2(action, __VA_ARGS__) action(__VA_ARGS__ __VA_OPT__(,) 15)
#define LOOP_17_2(action, ...) LOOP_16_2(action, __VA_ARGS__) action(__VA_ARGS__ __VA_OPT__(,) 16)
#define LOOP_18_2(action, ...) LOOP_17_2(action, __VA_ARGS__) action(__VA_ARGS__ __VA_OPT__(,) 17)
#define LOOP_19_2(action, ...) LOOP_18_2(action, __VA_ARGS__) action(__VA_ARGS__ __VA_OPT__(,) 18)
#define LOOP_20_2(action, ...) LOOP_19_2(action, __VA_ARGS__) action(__VA_ARGS__ __VA_OPT__(,) 19)
#define LOOP_21_2(action, ...) LOOP_20_2(action, __VA_ARGS__) action(__VA_ARGS__ __VA_OPT__(,) 20)
#define LOOP_22_2(action, ...) LOOP_21_2(action, __VA_ARGS__) action(__VA_ARGS__ __VA_OPT__(,) 21)
#define LOOP_23_2(action, ...) LOOP_22_2(action, __VA_ARGS__) action(__VA_ARGS__ __VA_OPT__(,) 22)
#define LOOP_24_2(action, ...) LOOP_23_2(action, __VA_ARGS__) action(__VA_ARGS__ __VA_OPT__(,) 23)
#define LOOP_25_2(action, ...) LOOP_24_2(action, __VA_ARGS__) action(__VA_ARGS__ __VA_OPT__(,) 24)
#define LOOP_26_2(action, ...) LOOP_25_2(action, __VA_ARGS__) action(__VA_ARGS__ __VA_OPT__(,) 25)
#define LOOP_27_2(action, ...) LOOP_26_2(action, __VA_ARGS__) action(__VA_ARGS__ __VA_OPT__(,) 26)
#define LOOP_28_2(action, ...) LOOP_27_2(action, __VA_ARGS__) action(__VA_ARGS__ __VA_OPT__(,) 27)
#define LOOP_29_2(action, ...) LOOP_28_2(action, __VA_ARGS__) action(__VA_ARGS__ __VA_OPT__(,) 28)
#define LOOP_30_2(action, ...) LOOP_29_2(action, __VA_ARGS__) action(__VA_ARGS__ __VA_OPT__(,) 29)
#define LOOP_31_2(action, ...) LOOP_30_2(action, __VA_ARGS__) action(__VA_ARGS__ __VA_OPT__(,) 30)
#define LOOP_32_2(action, ...) LOOP_31_2(action, __VA_ARGS__) action(__VA_ARGS__ __VA_OPT__(,) 31)
#define LOOP_33_2(action, ...) LOOP_32_2(action, __VA_ARGS__) action(__VA_ARGS__ __VA_OPT__(,) 32)
#define LOOP_34_2(action, ...) LOOP_33_2(action, __VA_ARGS__) action(__VA_ARGS__ __VA_OPT__(,) 33)
#define LOOP_35_2(action, ...) LOOP_34_2(action, __VA_ARGS__) action(__VA_ARGS__ __VA_OPT__(,) 34)
#define LOOP_36_2(action, ...) LOOP_35_2(action, __VA_ARGS__) action(__VA_ARGS__ __VA_OPT__(,) 35)
#define LOOP_37_2(action, ...) LOOP_36_2(action, __VA_ARGS__) action(__VA_ARGS__ __VA_OPT__(,) 36)
#define LOOP_38_2(action, ...) LOOP_37_2(action, __VA_ARGS__) action(__VA_ARGS__ __VA_OPT__(,) 37)
#define LOOP_39_2(action, ...) LOOP_38_2(action, __VA_ARGS__) action(__VA_ARGS__ __VA_OPT__(,) 38)
#define LOOP_40_2(action, ...) LOOP_39_2(action, __VA_ARGS__) action(__VA_ARGS__ __VA_OPT__(,) 39)
#define LOOP_41_2(action, ...) LOOP_40_2(action, __VA_ARGS__) action(__VA_ARGS__ __VA_OPT__(,) 40)
#define LOOP_42_2(action, ...) LOOP_41_2(action, __VA_ARGS__) action(__VA_ARGS__ __VA_OPT__(,) 41)
#define LOOP_43_2(action, ...) LOOP_42_2(action, __VA_ARGS__) action(__VA_ARGS__ __VA_OPT__(,) 42)
#define LOOP_44_2(action, ...) LOOP_43_2(action, __VA_ARGS__) action(__VA_ARGS__ __VA_OPT__(,) 43)
#define LOOP_45_2(action, ...) LOOP_44_2(action, __VA_ARGS__) action(__VA_ARGS__ __VA_OPT__(,) 44)
#define LOOP_46_2(action, ...) LOOP_45_2(action, __VA_ARGS__) action(__VA_ARGS__ __VA_OPT__(,) 45)
#define LOOP_47_2(action, ...) LOOP_46_2(action, __VA_ARGS__) action(__VA_ARGS__ __VA_OPT__(,) 46)
#define LOOP_48_2(action, ...) LOOP_47_2(action, __VA_ARGS__) action(__VA_ARGS__ __VA_OPT__(,) 47)
#define LOOP_49_2(action, ...) LOOP_48_2(action, __VA_ARGS__) action(__VA_ARGS__ __VA_OPT__(,) 48)
#define LOOP_50_2(action, ...) LOOP_49_2(action, __VA_ARGS__) action(__VA_ARGS__ __VA_OPT__(,) 49)
#define LOOP_51_2(action, ...) LOOP_50_2(action, __VA_ARGS__) action(__VA_ARGS__ __VA_OPT__(,) 50)
#define LOOP_52_2(action, ...) LOOP_51_2(action, __VA_ARGS__) action(__VA_ARGS__ __VA_OPT__(,) 51)
#define LOOP_53_2(action, ...) LOOP_52_2(action, __VA_ARGS__) action(__VA_ARGS__ __VA_OPT__(,) 52)
#define LOOP_54_2(action, ...) LOOP_53_2(action, __VA_ARGS__) action(__VA_ARGS__ __VA_OPT__(,) 53)
#define LOOP_55_2(action, ...) LOOP_54_2(action, __VA_ARGS__) action(__VA_ARGS__ __VA_OPT__(,) 54)
#define LOOP_56_2(action, ...) LOOP_55_2(action, __VA_ARGS__) action(__VA_ARGS__ __VA_OPT__(,) 55)
#define LOOP_57_2(action, ...) LOOP_56_2(action, __VA_ARGS__) action(__VA_ARGS__ __VA_OPT__(,) 56)
#define LOOP_58_2(action, ...) LOOP_57_2(action, __VA_ARGS__) action(__VA_ARGS__ __VA_OPT__(,) 57)
#define LOOP_59_2(action, ...) LOOP_58_2(action, __VA_ARGS__) action(__VA_ARGS__ __VA_OPT__(,) 58)
#define LOOP_60_2(action, ...) LOOP_59_2(action, __VA_ARGS__) action(__VA_ARGS__ __VA_OPT__(,) 59)
#define LOOP_61_2(action, ...) LOOP_60_2(action, __VA_ARGS__) action(__VA_ARGS__ __VA_OPT__(,) 60)
#define LOOP_62_2(action, ...) LOOP_61_2(action, __VA_ARGS__) action(__VA_ARGS__ __VA_OPT__(,) 61)
#define LOOP_63_2(action, ...) LOOP_62_2(action, __VA_ARGS__) action(__VA_ARGS__ __VA_OPT__(,) 62)
#define LOOP_64_2(action, ...) LOOP_63_2(action, __VA_ARGS__) action(__VA_ARGS__ __VA_OPT__(,) 63)

#define IF_NOT_EQUAL_0_0(  action)
#define IF_NOT_EQUAL_0_1(  action) action
#define IF_NOT_EQUAL_0_2(  action) action
#define IF_NOT_EQUAL_0_3(  action) action
#define IF_NOT_EQUAL_0_4(  action) action
#define IF_NOT_EQUAL_0_5(  action) action
#define IF_NOT_EQUAL_0_6(  action) action
#define IF_NOT_EQUAL_0_7(  action) action
#define IF_NOT_EQUAL_0_8(  action) action
#define IF_NOT_EQUAL_0_9(  action) action
#define IF_NOT_EQUAL_0_10( action) action
#define IF_NOT_EQUAL_0_11( action) action
#define IF_NOT_EQUAL_0_12( action) action
#define IF_NOT_EQUAL_0_13( action) action
#define IF_NOT_EQUAL_0_14( action) action
#define IF_NOT_EQUAL_0_15( action) action
#define IF_NOT_EQUAL_1_0(  action) action
#define IF_NOT_EQUAL_1_1(  action)
#define IF_NOT_EQUAL_1_2(  action) action
#define IF_NOT_EQUAL_1_3(  action) action
#define IF_NOT_EQUAL_1_4(  action) action
#define IF_NOT_EQUAL_1_5(  action) action
#define IF_NOT_EQUAL_1_6(  action) action
#define IF_NOT_EQUAL_1_7(  action) action
#define IF_NOT_EQUAL_1_8(  action) action
#define IF_NOT_EQUAL_1_9(  action) action
#define IF_NOT_EQUAL_1_10( action) action
#define IF_NOT_EQUAL_1_11( action) action
#define IF_NOT_EQUAL_1_12( action) action
#define IF_NOT_EQUAL_1_13( action) action
#define IF_NOT_EQUAL_1_14( action) action
#define IF_NOT_EQUAL_1_15( action) action
#define IF_NOT_EQUAL_2_0(  action) action
#define IF_NOT_EQUAL_2_1(  action) action
#define IF_NOT_EQUAL_2_2(  action)
#define IF_NOT_EQUAL_2_3(  action) action
#define IF_NOT_EQUAL_2_4(  action) action
#define IF_NOT_EQUAL_2_5(  action) action
#define IF_NOT_EQUAL_2_6(  action) action
#define IF_NOT_EQUAL_2_7(  action) action
#define IF_NOT_EQUAL_2_8(  action) action
#define IF_NOT_EQUAL_2_9(  action) action
#define IF_NOT_EQUAL_2_10( action) action
#define IF_NOT_EQUAL_2_11( action) action
#define IF_NOT_EQUAL_2_12( action) action
#define IF_NOT_EQUAL_2_13( action) action
#define IF_NOT_EQUAL_2_14( action) action
#define IF_NOT_EQUAL_2_15( action) action
#define IF_NOT_EQUAL_3_0(  action) action
#define IF_NOT_EQUAL_3_1(  action) action
#define IF_NOT_EQUAL_3_2(  action) action
#define IF_NOT_EQUAL_3_3(  action)
#define IF_NOT_EQUAL_3_4(  action) action
#define IF_NOT_EQUAL_3_5(  action) action
#define IF_NOT_EQUAL_3_6(  action) action
#define IF_NOT_EQUAL_3_7(  action) action
#define IF_NOT_EQUAL_3_8(  action) action
#define IF_NOT_EQUAL_3_9(  action) action
#define IF_NOT_EQUAL_3_10( action) action
#define IF_NOT_EQUAL_3_11( action) action
#define IF_NOT_EQUAL_3_12( action) action
#define IF_NOT_EQUAL_3_13( action) action
#define IF_NOT_EQUAL_3_14( action) action
#define IF_NOT_EQUAL_3_15( action) action
#define IF_NOT_EQUAL_4_0(  action) action
#define IF_NOT_EQUAL_4_1(  action) action
#define IF_NOT_EQUAL_4_2(  action) action
#define IF_NOT_EQUAL_4_3(  action) action
#define IF_NOT_EQUAL_4_4(  action)
#define IF_NOT_EQUAL_4_5(  action) action
#define IF_NOT_EQUAL_4_6(  action) action
#define IF_NOT_EQUAL_4_7(  action) action
#define IF_NOT_EQUAL_4_8(  action) action
#define IF_NOT_EQUAL_4_9(  action) action
#define IF_NOT_EQUAL_4_10( action) action
#define IF_NOT_EQUAL_4_11( action) action
#define IF_NOT_EQUAL_4_12( action) action
#define IF_NOT_EQUAL_4_13( action) action
#define IF_NOT_EQUAL_4_14( action) action
#define IF_NOT_EQUAL_4_15( action) action
#define IF_NOT_EQUAL_5_0(  action) action
#define IF_NOT_EQUAL_5_1(  action) action
#define IF_NOT_EQUAL_5_2(  action) action
#define IF_NOT_EQUAL_5_3(  action) action
#define IF_NOT_EQUAL_5_4(  action) action
#define IF_NOT_EQUAL_5_5(  action)
#define IF_NOT_EQUAL_5_6(  action) action
#define IF_NOT_EQUAL_5_7(  action) action
#define IF_NOT_EQUAL_5_8(  action) action
#define IF_NOT_EQUAL_5_9(  action) action
#define IF_NOT_EQUAL_5_10( action) action
#define IF_NOT_EQUAL_5_11( action) action
#define IF_NOT_EQUAL_5_12( action) action
#define IF_NOT_EQUAL_5_13( action) action
#define IF_NOT_EQUAL_5_14( action) action
#define IF_NOT_EQUAL_5_15( action) action
#define IF_NOT_EQUAL_6_0(  action) action
#define IF_NOT_EQUAL_6_1(  action) action
#define IF_NOT_EQUAL_6_2(  action) action
#define IF_NOT_EQUAL_6_3(  action) action
#define IF_NOT_EQUAL_6_4(  action) action
#define IF_NOT_EQUAL_6_5(  action) action
#define IF_NOT_EQUAL_6_6(  action)
#define IF_NOT_EQUAL_6_7(  action) action
#define IF_NOT_EQUAL_6_8(  action) action
#define IF_NOT_EQUAL_6_9(  action) action
#define IF_NOT_EQUAL_6_10( action) action
#define IF_NOT_EQUAL_6_11( action) action
#define IF_NOT_EQUAL_6_12( action) action
#define IF_NOT_EQUAL_6_13( action) action
#define IF_NOT_EQUAL_6_14( action) action
#define IF_NOT_EQUAL_6_15( action) action
#define IF_NOT_EQUAL_7_0(  action) action
#define IF_NOT_EQUAL_7_1(  action) action
#define IF_NOT_EQUAL_7_2(  action) action
#define IF_NOT_EQUAL_7_3(  action) action
#define IF_NOT_EQUAL_7_4(  action) action
#define IF_NOT_EQUAL_7_5(  action) action
#define IF_NOT_EQUAL_7_6(  action) action
#define IF_NOT_EQUAL_7_7(  action)
#define IF_NOT_EQUAL_7_8(  action) action
#define IF_NOT_EQUAL_7_9(  action) action
#define IF_NOT_EQUAL_7_10( action) action
#define IF_NOT_EQUAL_7_11( action) action
#define IF_NOT_EQUAL_7_12( action) action
#define IF_NOT_EQUAL_7_13( action) action
#define IF_NOT_EQUAL_7_14( action) action
#define IF_NOT_EQUAL_7_15( action) action
#define IF_NOT_EQUAL_8_0(  action) action
#define IF_NOT_EQUAL_8_1(  action) action
#define IF_NOT_EQUAL_8_2(  action) action
#define IF_NOT_EQUAL_8_3(  action) action
#define IF_NOT_EQUAL_8_4(  action) action
#define IF_NOT_EQUAL_8_5(  action) action
#define IF_NOT_EQUAL_8_6(  action) action
#define IF_NOT_EQUAL_8_7(  action) action
#define IF_NOT_EQUAL_8_8(  action)
#define IF_NOT_EQUAL_8_9(  action) action
#define IF_NOT_EQUAL_8_10( action) action
#define IF_NOT_EQUAL_8_11( action) action
#define IF_NOT_EQUAL_8_12( action) action
#define IF_NOT_EQUAL_8_13( action) action
#define IF_NOT_EQUAL_8_14( action) action
#define IF_NOT_EQUAL_8_15( action) action
#define IF_NOT_EQUAL_9_0(  action) action
#define IF_NOT_EQUAL_9_1(  action) action
#define IF_NOT_EQUAL_9_2(  action) action
#define IF_NOT_EQUAL_9_3(  action) action
#define IF_NOT_EQUAL_9_4(  action) action
#define IF_NOT_EQUAL_9_5(  action) action
#define IF_NOT_EQUAL_9_6(  action) action
#define IF_NOT_EQUAL_9_7(  action) action
#define IF_NOT_EQUAL_9_8(  action) action
#define IF_NOT_EQUAL_9_9(  action)
#define IF_NOT_EQUAL_9_10( action) action
#define IF_NOT_EQUAL_9_11( action) action
#define IF_NOT_EQUAL_9_12( action) action
#define IF_NOT_EQUAL_9_13( action) action
#define IF_NOT_EQUAL_9_14( action) action
#define IF_NOT_EQUAL_9_15( action) action
#define IF_NOT_EQUAL_10_0( action) action
#define IF_NOT_EQUAL_10_1( action) action
#define IF_NOT_EQUAL_10_2( action) action
#define IF_NOT_EQUAL_10_3( action) action
#define IF_NOT_EQUAL_10_4( action) action
#define IF_NOT_EQUAL_10_5( action) action
#define IF_NOT_EQUAL_10_6( action) action
#define IF_NOT_EQUAL_10_7( action) action
#define IF_NOT_EQUAL_10_8( action) action
#define IF_NOT_EQUAL_10_9( action) action
#define IF_NOT_EQUAL_10_10(action)
#define IF_NOT_EQUAL_10_11(action) action
#define IF_NOT_EQUAL_10_12(action) action
#define IF_NOT_EQUAL_10_13(action) action
#define IF_NOT_EQUAL_10_14(action) action
#define IF_NOT_EQUAL_10_15(action) action
#define IF_NOT_EQUAL_11_0( action) action
#define IF_NOT_EQUAL_11_1( action) action
#define IF_NOT_EQUAL_11_2( action) action
#define IF_NOT_EQUAL_11_3( action) action
#define IF_NOT_EQUAL_11_4( action) action
#define IF_NOT_EQUAL_11_5( action) action
#define IF_NOT_EQUAL_11_6( action) action
#define IF_NOT_EQUAL_11_7( action) action
#define IF_NOT_EQUAL_11_8( action) action
#define IF_NOT_EQUAL_11_9( action) action
#define IF_NOT_EQUAL_11_10(action) action
#define IF_NOT_EQUAL_11_11(action)
#define IF_NOT_EQUAL_11_12(action) action
#define IF_NOT_EQUAL_11_13(action) action
#define IF_NOT_EQUAL_11_14(action) action
#define IF_NOT_EQUAL_11_15(action) action
#define IF_NOT_EQUAL_12_0( action) action
#define IF_NOT_EQUAL_12_1( action) action
#define IF_NOT_EQUAL_12_2( action) action
#define IF_NOT_EQUAL_12_3( action) action
#define IF_NOT_EQUAL_12_4( action) action
#define IF_NOT_EQUAL_12_5( action) action
#define IF_NOT_EQUAL_12_6( action) action
#define IF_NOT_EQUAL_12_7( action) action
#define IF_NOT_EQUAL_12_8( action) action
#define IF_NOT_EQUAL_12_9( action) action
#define IF_NOT_EQUAL_12_10(action) action
#define IF_NOT_EQUAL_12_11(action) action
#define IF_NOT_EQUAL_12_12(action)
#define IF_NOT_EQUAL_12_13(action) action
#define IF_NOT_EQUAL_12_14(action) action
#define IF_NOT_EQUAL_12_15(action) action
#define IF_NOT_EQUAL_13_0( action) action
#define IF_NOT_EQUAL_13_1( action) action
#define IF_NOT_EQUAL_13_2( action) action
#define IF_NOT_EQUAL_13_3( action) action
#define IF_NOT_EQUAL_13_4( action) action
#define IF_NOT_EQUAL_13_5( action) action
#define IF_NOT_EQUAL_13_6( action) action
#define IF_NOT_EQUAL_13_7( action) action
#define IF_NOT_EQUAL_13_8( action) action
#define IF_NOT_EQUAL_13_9( action) action
#define IF_NOT_EQUAL_13_10(action) action
#define IF_NOT_EQUAL_13_11(action) action
#define IF_NOT_EQUAL_13_12(action) action
#define IF_NOT_EQUAL_13_13(action)
#define IF_NOT_EQUAL_13_14(action) action
#define IF_NOT_EQUAL_13_15(action) action
#define IF_NOT_EQUAL_14_0( action) action
#define IF_NOT_EQUAL_14_1( action) action
#define IF_NOT_EQUAL_14_2( action) action
#define IF_NOT_EQUAL_14_3( action) action
#define IF_NOT_EQUAL_14_4( action) action
#define IF_NOT_EQUAL_14_5( action) action
#define IF_NOT_EQUAL_14_6( action) action
#define IF_NOT_EQUAL_14_7( action) action
#define IF_NOT_EQUAL_14_8( action) action
#define IF_NOT_EQUAL_14_9( action) action
#define IF_NOT_EQUAL_14_10(action) action
#define IF_NOT_EQUAL_14_11(action) action
#define IF_NOT_EQUAL_14_12(action) action
#define IF_NOT_EQUAL_14_13(action) action
#define IF_NOT_EQUAL_14_14(action)
#define IF_NOT_EQUAL_14_15(action) action
#define IF_NOT_EQUAL_15_0( action) action
#define IF_NOT_EQUAL_15_1( action) action
#define IF_NOT_EQUAL_15_2( action) action
#define IF_NOT_EQUAL_15_3( action) action
#define IF_NOT_EQUAL_15_4( action) action
#define IF_NOT_EQUAL_15_5( action) action
#define IF_NOT_EQUAL_15_6( action) action
#define IF_NOT_EQUAL_15_7( action) action
#define IF_NOT_EQUAL_15_8( action) action
#define IF_NOT_EQUAL_15_9( action) action
#define IF_NOT_EQUAL_15_10(action) action
#define IF_NOT_EQUAL_15_11(action) action
#define IF_NOT_EQUAL_15_12(action) action
#define IF_NOT_EQUAL_15_13(action) action
#define IF_NOT_EQUAL_15_14(action) action
#define IF_NOT_EQUAL_15_15(action)

// clang-format on
