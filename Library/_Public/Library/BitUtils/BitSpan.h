// copyright Daniel Dahlkvist (c) 2020
#pragma once

#include <Core/Platform.h>
#include <Core/Types.h>
#include <Library/library_module.h>
#include <algorithm>

namespace ddahlkvist
{

using BitWordType = u64;
constexpr u32 BitsInWord = sizeof(BitWordType) * 8;

constexpr BitWordType numBitsToMask(u32 numBits)
{
	DD_ASSERT(numBits <= BitsInWord);

	if (numBits == BitsInWord)
		return BitWordType{ ~0ULL };

	BitWordType value = 1;
	value <<= numBits;
	value -= 1;
	return value;
}


// utility class for being able to perform actions on two different "range of bits"
struct BitRangeZipper final
{
	BitRangeZipper(void* __restrict data, void* __restrict data2, u32 bitCount)
		: _data(reinterpret_cast<BitWordType*>(data))
		, _data2(reinterpret_cast<BitWordType*>(data2))
		, _bitCount(bitCount)
		, _danglingMask(numBitsToMask(bitCount % 64))
		, _iterations(static_cast<u16>(bitCount / 64))
	{
	}

	// examples:
	// auto operatorOrEq = [](auto& a, auto b) -> BitWordType { a |= b; }
	// auto danglingOrEq = [](auto& a, auto b, auto mask) -> BitWordType { return a = ((a|b) & mask) | (a & ~mask); }
	// zipper.foreachWord(operatorOrEq, danglingOrEq);
	template<typename WordFunctor, typename DanglingWordFunctor>
	inline void foreachWord(WordFunctor&& func, DanglingWordFunctor&& danglingFunc) noexcept {
		auto it1 = _data;
		auto it2 = _data2;
		auto end = it1 + _iterations;

		while (it1 != end)
		{
			func(*it1, *it2);
			it1++;
			it2++;
		}

		if (auto mask = numBitsToMask(_bitCount % BitsInWord))
			danglingFunc(*it1, *it2, mask);
	}

private:
	BitWordType* __restrict _data;
	BitWordType* __restrict _data2;
	BitWordType _danglingMask;
	u32 _bitCount;
	u16 _iterations;
};

// provides bit-wise commands on top of range of bytes that it does not own memory for
class BitSpan final
{
public:
	inline BitSpan(void* data, u32 bitCount)
		: _data(reinterpret_cast<BitWordType*>(data))
		, _numBits(bitCount)
		, _numWords(_numBits / BitsInWord)
		, _danglingMask(numBitsToMask(_numBits % BitsInWord) )
	{ }

	inline void clearAll()
	{
		auto it = _data;
		auto end = _data + _numWords;
		const BitWordType Zero = BitWordType{ 0 };
		if (end > it)
		{
			std::fill(it, end, Zero);
		}

		if (_danglingMask)
		{
			const BitWordType part2 = (*end & ~_danglingMask);
			const BitWordType dangling = part2;
			*end = dangling;
		}
	}

	inline void setAll()
	{
		auto it = _data;
		auto end = _data + _numWords;
		const BitWordType Ones = BitWordType{ ~0ull };
		if (end > it)
		{
			std::fill(it, end, Ones);
		}

		if (_danglingMask)
		{
			const BitWordType part1 = (Ones & _danglingMask);
			const BitWordType part2 = (*end & ~_danglingMask);
			const BitWordType dangling = part1 | part2;
			*end = dangling; 
		}
	}

private:
	BitWordType* _data;

	const u32 _numBits;
	const u32 _numWords;

	const BitWordType _danglingMask;
};

}
