#pragma once

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

}
}