// copyright Daniel Dahlkvist (c) 2020 [github.com/messer1024]
#pragma once

#include <Core/Platform.h>
#include <Core/Types.h>
#include <Library/library_module.h>

namespace ddahlkvist
{

using BitWordType = u64;
constexpr u32 BitsInWord = sizeof(BitWordType) * 8;

namespace bitword
{

constexpr BitWordType Zero = BitWordType{ 0 };
constexpr BitWordType Ones = BitWordType{ ~0ull };

constexpr BitWordType danglingPart(u32 numBits)
{
	numBits = numBits % BitsInWord;

	BitWordType value = 1;
	value <<= numBits;
	value -= 1;
	return value;
}

//template<class WordAction>
//	void foreachSetBit(WordAction&& action, BitWordType word, uint invokedBitIndexOffset = 0)
//	{
//		uint i = invokedBitIndexOffset;
//
//		while (word != 0u)
//		{
//			if (word & 1u)
//				action(i);
//
//			i++;
//			word >>= 1;
//		}
//	}
//
//	inline bool equals(BitWordType a, BitWordType b, u32 numBitsToCompare)
//	{
//		const BitWordType mask = danglingPart(numBitsToCompare);
//		return (a & mask) == (b & mask);
//	}
}

// utility class for being able to perform actions on two different "range of bits"
class BitRangeZipper final
{
public:
	BitRangeZipper(void* __restrict lhs, void* __restrict rhs, u32 numBits)
		: _lhs(reinterpret_cast<BitWordType*>(lhs))
		, _rhs(reinterpret_cast<BitWordType*>(rhs))
		, _danglingMask((numBits% BitsInWord != 0) ? bitword::danglingPart(numBits) : ~0ull)
		, _numWords((numBits / BitsInWord) + (numBits % BitsInWord != 0))
		, _numBits(numBits)
	{
		clearDanglingBits();
	}

	inline void clearDanglingBits()
	{
		if (_numWords > 0) {
			_lhs[_numWords - 1] &= _danglingMask;
			_rhs[_numWords - 1] &= _danglingMask;
		}
	}

	// examples:
	// auto operatorOREq = [](auto& a, auto b) -> BitWordType { a |= b; }
	// auto danglingOREq = [](auto& a, auto b, auto mask) -> BitWordType { return a = ((a|b) & mask) | (a & ~mask); }
	// zipper.foreachWord(operatorOrEq, danglingOrEq);
	template<class WordAction>
	inline void foreachWord(WordAction&& action) noexcept {
		auto lhs = _lhs;
		auto rhs = _rhs;
		auto end = lhs + _numWords;

		while (lhs != end)
		{
			action(*lhs, *rhs);
			lhs++;
			rhs++;
		}
	}

private:
	BitWordType* __restrict _lhs;
	BitWordType* __restrict _rhs;
	BitWordType _danglingMask;

	const u32 _numWords;
	const u32 _numBits;
};

// BitSpan provide functionality to reason about a range of bits
// it does not own or manage any data/buffer [memory management is supposed to happen outside of this class]
// will attempt to "zero" any eventual dangling bits [seems like the best trade-off related to usability, performance and correctness]
class BitSpan final
{
public:
	inline BitSpan(void* data, u32 numBits)
		: _data(reinterpret_cast<BitWordType*>(data))
		, _danglingMask((numBits% BitsInWord != 0) ? bitword::danglingPart(numBits) : ~0ull)
		, _numWords((numBits / BitsInWord) + (numBits % BitsInWord != 0))
		, _numBits(numBits)
	{
		DD_ASSERT(numBits < 400000000); // sanity check against "-1 issues"
		clearDanglingBits();
	}

	inline void clearDanglingBits()
	{
		if (_numWords > 0)
			_data[_numWords - 1] &= _danglingMask;
	}

	inline void clearAll() noexcept
	{
		foreachWord([](auto& a) { a = BitWordType{ 0 }; });
	}

	inline void setAll() noexcept
	{
		foreachWord([](auto& a) { a = BitWordType{ ~0ull }; });

		clearDanglingBits();
	}

	template<typename WordAction>
	inline void foreachWord(WordAction&& action) noexcept {
		auto it = _data;
		auto end = it + _numWords;

		while (it != end)
		{
			action(*it);
			it++;
		}
	}

private:
	BitWordType* _data;
	BitWordType _danglingMask;

	const u32 _numWords;
	const u32 _numBits;
};

}
