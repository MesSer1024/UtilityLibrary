#pragma once

#include <Core/Platform.h>
#include <Core/Types.h>
#include <Library/library_module.h>
#include <algorithm>

namespace ddahlkvist
{

using BitWordType = u64;
constexpr u32 BitsInWord = sizeof(BitWordType) * 8;

constexpr BitWordType bitCountToMask(u32 numBits)
{
	DD_ASSERT(numBits <= BitsInWord);

	if (numBits == BitsInWord)
		return BitWordType{ ~0ULL };

	BitWordType value = 1;
	value <<= numBits;
	value -= 1;
	return value;
}


//
//struct BitRangeZipper final
//{
//	BitRangeZipper(void* __restrict data, void* __restrict data2, u32 bitCount)
//		: _data(reinterpret_cast<BitWordType*>(data))
//		, _data2(reinterpret_cast<BitWordType*>(data2))
//		, _bitCount(bitCount)
//		, _dangling(_bitCount % 64 != 0)
//		, _iterations(static_cast<u16>(_bitCount / 64))
//	{
//	}
//
//	template<typename TemplateEachWordAction, typename TemplateDanglingWordAction>
//	inline void foreachWord(TemplateEachWordAction&& action, TemplateDanglingWordAction&& danglingAction) noexcept {
//		auto it1 = _data;
//		auto it2 = _data2;
//		auto end = it1 + _iterations;
//		while (it1 != end)
//		{
//			*it1 = action(*it1, *it2);
//			it1++;
//			it2++;
//		}
//
//		if (_dangling)
//		{
//			auto mask = bitCountToMask(_bitCount);
//			*it1 = danglingAction(*it1, *it2, mask);
//		}
//	}
//
//private:
//	BitWordType* __restrict _data;
//	BitWordType* __restrict _data2;
//	u32 _bitCount;
//	u16 _dangling;
//	u16 _iterations;
//};

// provides bit-wise commands on top of range of bytes that it does not own memory for
class BitSpan final
{
public:
	inline BitSpan(void* data, u32 bitCount)
		: _data(reinterpret_cast<BitWordType*>(data))
		, _numBits(bitCount)
		, _numWords(_numBits / BitsInWord)
		, _danglingMask(bitCountToMask(_numBits % BitsInWord) )
	{ }

	inline void clearBits()
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
			BitWordType dangling = *end & ~_danglingMask;
			*end = dangling;
		}
	}

	inline void setBits()
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
			BitWordType dangling = (*end & ~_danglingMask) | (Ones & _danglingMask);
			*end = dangling;
		}
	}

	//void foo()
	//{
	//	u64 sadf[5];
	//	BitRangeZipper zipper(sadf, sadf, 137);
	//}

	/*
	template<class Action>
	void foreachSetBit(Action&& action)
	{
		BitWordType* wordPtr = reinterpret_cast<BitWordType*>(_data);

		int globalBitIndex = 0;
		int bitIndex;
		for (u32 it = 0; it < _numWords; ++it)
		{
			BitWordType value = *wordPtr;
			bitIndex = globalBitIndex;
			while (value != 0u)
			{
				const bool smallestBitIsSet = value & 1u;
				if (smallestBitIsSet)
					action(bitIndex);

				value >>= 1u;
				bitIndex++;
			}

			globalBitIndex += BitsInWord;
			wordPtr++;
		}

		if (_danglingMask != 0)
		{
			const BitWordType mask = bitCountToMask(_danglingMask);
			BitWordType value = (*wordPtr) & mask;
			bitIndex = globalBitIndex;
			while (value != 0u)
			{
				const bool smallestBitIsSet = value & 1u;
				if (smallestBitIsSet)
					action(bitIndex);

				value >>= 1u;
				bitIndex++;
			}

		}
	}*/

private:
	BitWordType* _data;

	const u32 _numBits;
	const u32 _numWords;

	const BitWordType _danglingMask;
};

}
