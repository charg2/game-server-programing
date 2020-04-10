#pragma once
#include <algorithm>
#include <type_traits>

template <typename Type>
Type Clamp(Type min, Type x, Type max)
{
	static_assert(std::is_arithmetic<Type>::value, L"Type must pod!!");
	return std::max(min, std::min(x, max));
}