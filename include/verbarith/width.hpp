#pragma once

#include <climits>
#include <type_traits>

#define widthof(T) (std::is_same_v<std::remove_cvref_t<T>, bool> ? 1 : sizeof(T) * CHAR_BIT)
