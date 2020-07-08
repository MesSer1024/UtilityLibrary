#pragma once

#include <algorithm>
#include <numeric>

namespace ddahlkvist
{
namespace meta
{

template<class Container, class Value>
void fill_container(Container& container, const Value& value)
{
	auto begin = std::begin(container);
	auto end = std::end(container);
	std::fill(begin, end, value);
}

template<class Container, class Value>
void iota_container(Container& container, Value firstValue = 0)
{
	auto begin = std::begin(container);
	auto end = std::end(container);
	std::iota(begin, end, firstValue);
}

}
}