#pragma once
#include <algorithm>
#include <type_traits>

template <typename Type>
Type my_clamp(Type min_v, Type x, Type max_v)
{
	//static_assert(std::is_arithmetic<Type>::value, L"Type must pod!!");
	return max( min_v, min(x, max_v));
}