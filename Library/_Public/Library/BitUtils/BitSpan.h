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

		}*/
	}

private:
	BitWordType* _data;

	const u32 _numBits;
	const u32 _numWords;

	const BitWordType _danglingMask;
};

}
