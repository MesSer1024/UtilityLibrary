#pragma once

#include <Core/Platform.h>
#include <Core/Types.h>
#include <Library/library_module.h>

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
	LIBRARY_PUBLIC BitSpan(void* data, u32 bitCount);

	//using BitAction = std::function<void(u32 bitIndex)>;

	//template <typename T>
	//constexpr inline void foreachSetBit(const T* __restrict source, BitAction action) {
	//	T word = source[0];

	//	for (u32 i = 0u; word != 0u; ++i) {
	//		const bool smallestBitIsSet = word & T(1u);
	//		if (smallestBitIsSet) {
	//			action(i);
	//		}

	//		word >>= 1u;
	//	}
	//}


	template<class Action>
	void foreachSetBit(Action&& action)
	{
		BitWordType* wordPtr = reinterpret_cast<BitWordType*>(_data);

		const u32 wordIterations = _bitCount / BitsInWord;
		const u32 danglingBits = _bitCount & BitsInWord;

		int globalBitIndex = 0;
		int bitIndex;
		for (u32 it = 0; it < wordIterations; ++it)
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

		if (danglingBits != 0)
		{
			const BitWordType mask = bitCountToMask(danglingBits);
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
	}

private:
	void* _data;
	u32 _bitCount;
};

}
