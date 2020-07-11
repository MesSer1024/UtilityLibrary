// copyright Daniel Dahlkvist (c) 2020 [github.com/messer1024]
#include <Library/BitUtils/BitSpan.h>

#include <Core/Meta/Meta.h>
#include <Core/Types.h>
#include <gtest/gtest.h>

namespace ddahlkvist
{

static_assert(sizeof(BitWordType) == sizeof(u64));

TEST(bitword_fixture, danglingMaskFromBitCount_validateCalculations) {
	using namespace bitword;
	{
		constexpr BitWordType mask = bitword::getDanglingPart(0);
		ASSERT_EQ(mask, BitWordType{ 0u });
	}

	{
		constexpr BitWordType mask = bitword::getDanglingPart(1);
		ASSERT_EQ(mask, BitWordType{ 1u });
	}

	{
		constexpr BitWordType mask = bitword::getDanglingPart(2);
		ASSERT_EQ(mask, BitWordType{ 0b11 });
	}

	{
		constexpr BitWordType mask = bitword::getDanglingPart(3);
		ASSERT_EQ(mask, BitWordType{ 0b111 });
	}
	{
		constexpr BitWordType mask = bitword::getDanglingPart(4);
		ASSERT_EQ(mask, BitWordType{ 0xF });
	}

	{
		constexpr BitWordType mask = bitword::getDanglingPart(6);
		ASSERT_EQ(mask, BitWordType{ 0x3f });
	}

	{
		constexpr BitWordType mask = bitword::getDanglingPart(8);
		ASSERT_EQ(mask, BitWordType{ 0xFF });
	}

	{
		constexpr BitWordType mask = bitword::getDanglingPart(32);
		ASSERT_EQ(mask, BitWordType{ 0xFFFFFFFF });
	}

	{
		constexpr BitWordType mask = bitword::getDanglingPart(33);
		ASSERT_EQ(mask, BitWordType{ 0x1FFFFFFFF });
	}

	{
		constexpr BitWordType mask = bitword::getDanglingPart(63);
		ASSERT_EQ(mask, BitWordType{ 0x7FFFFFFFFFFFFFFF });
	}
	{
		constexpr BitWordType mask = bitword::getDanglingPart(64);
		ASSERT_EQ(mask, BitWordType{ 0 });
	}
	{
		constexpr BitWordType mask = bitword::getDanglingPart(65);
		ASSERT_EQ(mask, BitWordType{ 1 });
	}
	{
		constexpr BitWordType mask = bitword::getDanglingPart(130);
		ASSERT_EQ(mask, BitWordType{ 0b11 });
	}
}


TEST(bitword_fixture, foreachSetBit_invokedWithCorrectBitIndex)
{
	bool output[NumBitsInWord] = {};
	bool expected[NumBitsInWord] = {};

	BitWordType data = { 0 };
	BitWordType one = 1;

	expected[1] = expected[5] = expected[6] = expected[11] = expected[63] = true;

	for (uint i = 0; i < NumBitsInWord; ++i)
		if (expected[i])
			data |= (one << i);

	bitword::foreachSetBit([&output](u32 bitIdx) {
		output[bitIdx] = true;
		},
		data);

	for (uint i = 0; i < NumBitsInWord; ++i)
		ASSERT_EQ(output[i], expected[i]);
}

TEST(bitword_fixture, foreachSetBit_tresholds)
{
	BitWordType one = 1;

	{
		const u32 ExpectedBitIndex = 0;
		BitWordType data = one << ExpectedBitIndex;
		bitword::foreachSetBit([&](u32 bit) { ASSERT_EQ(bit, ExpectedBitIndex); }, data);
	}
	{
		const u32 ExpectedBitIndex = 1;
		BitWordType data = one << ExpectedBitIndex;
		bitword::foreachSetBit([&](u32 bit) { ASSERT_EQ(bit, ExpectedBitIndex); }, data);
	}
	{
		const u32 ExpectedBitIndex = 31;
		BitWordType data = one << ExpectedBitIndex;
		bitword::foreachSetBit([&](u32 bit) { ASSERT_EQ(bit, ExpectedBitIndex); }, data);
	}
	{
		const u32 ExpectedBitIndex = 32;
		BitWordType data = one << ExpectedBitIndex;
		bitword::foreachSetBit([&](u32 bit) { ASSERT_EQ(bit, ExpectedBitIndex); }, data);
	}
	{
		const u32 ExpectedBitIndex = 33;
		BitWordType data = one << ExpectedBitIndex;
		bitword::foreachSetBit([&](u32 bit) { ASSERT_EQ(bit, ExpectedBitIndex); }, data);
	}
	{
		const u32 ExpectedBitIndex = 63;
		BitWordType data = one << ExpectedBitIndex;
		bitword::foreachSetBit([&](u32 bit) { ASSERT_EQ(bit, ExpectedBitIndex); }, data);
	}
}

TEST(bitword_fixture, foreachSetBit_withReportedIndexOffset)
{
	BitWordType one = 1;
	const u32 IndexOffset = 17;
	{
		const u32 ExpectedBitIndex = 0;
		BitWordType data = one << ExpectedBitIndex;
		bitword::foreachSetBit([&](u32 bit) { ASSERT_EQ(bit, ExpectedBitIndex + IndexOffset); }, data, IndexOffset);
	}
	{
		const u32 ExpectedBitIndex = 14;
		BitWordType data = one << ExpectedBitIndex;
		bitword::foreachSetBit([&](u32 bit) { ASSERT_EQ(bit, ExpectedBitIndex + IndexOffset); }, data, IndexOffset);
	}
	{
		const u32 ExpectedBitIndex = 33;
		BitWordType data = one << ExpectedBitIndex;
		bitword::foreachSetBit([&](u32 bit) { ASSERT_EQ(bit, ExpectedBitIndex + IndexOffset); }, data, IndexOffset);
	}
	{
		const u32 ExpectedBitIndex = 63 - IndexOffset;
		BitWordType data = one << ExpectedBitIndex;
		bitword::foreachSetBit([&](u32 bit) { ASSERT_EQ(bit, ExpectedBitIndex + IndexOffset); }, data, IndexOffset);
	}
	{
		const u32 ExpectedBitIndex = 64 - IndexOffset;
		BitWordType data = one << ExpectedBitIndex;
		bitword::foreachSetBit([&](u32 bit) { ASSERT_EQ(bit, ExpectedBitIndex + IndexOffset); }, data, IndexOffset);
	}
	{
		const u32 ExpectedBitIndex = 55;
		BitWordType data = one << ExpectedBitIndex;
		bitword::foreachSetBit([&](u32 bit) { ASSERT_EQ(bit, ExpectedBitIndex + IndexOffset); }, data, IndexOffset);
	}
	{
		const u32 ExpectedBitIndex = 63;
		BitWordType data = one << ExpectedBitIndex;
		bitword::foreachSetBit([&](u32 bit) { ASSERT_EQ(bit, ExpectedBitIndex + IndexOffset); }, data, IndexOffset);
	}
}

TEST(bitword_fixture, foreachSetBit_withReportedIndexOffsetCanBeReallyBig)
{
	BitWordType one = 1;
	const u32 IndexOffset = 8181818;
	{
		const u32 ExpectedBitIndex = 0;
		BitWordType data = one << ExpectedBitIndex;
		bitword::foreachSetBit([&](u32 bit) { ASSERT_EQ(bit, ExpectedBitIndex + IndexOffset); }, data, IndexOffset);
	}
	{
		const u32 ExpectedBitIndex = 14;
		BitWordType data = one << ExpectedBitIndex;
		bitword::foreachSetBit([&](u32 bit) { ASSERT_EQ(bit, ExpectedBitIndex + IndexOffset); }, data, IndexOffset);
	}
	{
		const u32 ExpectedBitIndex = 33;
		BitWordType data = one << ExpectedBitIndex;
		bitword::foreachSetBit([&](u32 bit) { ASSERT_EQ(bit, ExpectedBitIndex + IndexOffset); }, data, IndexOffset);
	}
	{
		const u32 ExpectedBitIndex = 55;
		BitWordType data = one << ExpectedBitIndex;
		bitword::foreachSetBit([&](u32 bit) { ASSERT_EQ(bit, ExpectedBitIndex + IndexOffset); }, data, IndexOffset);
	}
	{
		const u32 ExpectedBitIndex = 63;
		BitWordType data = one << ExpectedBitIndex;
		bitword::foreachSetBit([&](u32 bit) { ASSERT_EQ(bit, ExpectedBitIndex + IndexOffset); }, data, IndexOffset);
	}
}

TEST(bitword_fixture, setbit13)
{
	BitWordType value{};

	bitword::setBit(value, 13);

	ASSERT_TRUE(bitword::getBit(value, 13));
}

TEST(bitword_fixture, setAndGet_testAllValidVariations)
{
	BitWordType one = { 1 };
	for (uint i = 0; i < NumBitsInWord; ++i)
	{
		const BitWordType expected = one << i;
		BitWordType value = 0;

		bitword::setBit(value, i);
		ASSERT_TRUE(bitword::getBit(value, i));

		ASSERT_EQ(expected, value);
	}
}

TEST(bitword_fixture, testSetAndGet_invalidValues)
{
	// all of these are UB
	//{
	//	BitWordType value = 0;
	//	bitword::setBit(value, 64);
	//	ASSERT_TRUE(bitword::getBit(value, 64));
	//}
	//{
	//	BitWordType value = 0;
	//	bitword::setBit(value, 65);
	//	ASSERT_TRUE(bitword::getBit(value, 65));
	//}
	//{
	//	BitWordType value = 0;
	//	bitword::setBit(value, 65);
	//	ASSERT_TRUE(bitword::getBit(value, 65));
	//}
}

#pragma warning( push )
#pragma warning( disable : 4293 ) // warning C4293: '<<': shift count negative or too big, undefined behavior

//TEST(bitword_fixture, foreachSetBit_shiftTooLarge_ub)
//{
	//BitWordType one = 1;

	// all of these are UB
	//{
	//	const u32 ExpectedBitIndex = 64;
	//	BitWordType data = one << ExpectedBitIndex;
	//	bitword::foreachSetBit([&](u32 bit) { ASSERT_EQ(bit, ExpectedBitIndex % NumBitsInWord); }, data);
	//}
	//{

	//	const u32 ExpectedBitIndex = 65;
	//	BitWordType data = one << ExpectedBitIndex;
	//	bitword::foreachSetBit([&](u32 bit) { ASSERT_EQ(bit, ExpectedBitIndex % NumBitsInWord); }, data);
	//}
	//{

	//	const u32 ExpectedBitIndex = 127;
	//	BitWordType data = one << ExpectedBitIndex;
	//	bitword::foreachSetBit([&](u32 bit) { ASSERT_EQ(bit, ExpectedBitIndex % NumBitsInWord); }, data);
	//}
	//{

	//	const u32 ExpectedBitIndex = 128;
	//	BitWordType data = one << ExpectedBitIndex;
	//	bitword::foreachSetBit([&](u32 bit) { ASSERT_EQ(bit, ExpectedBitIndex % NumBitsInWord); }, data);
	//}
	//{

	//	const u32 ExpectedBitIndex = 129;
	//	BitWordType data = one << ExpectedBitIndex;
	//	bitword::foreachSetBit([&](u32 bit) { ASSERT_EQ(bit, ExpectedBitIndex % NumBitsInWord); }, data);
	//}
//}

TEST(bitword_fixture, foreachSetBit_weirdCastBehavior_andUB)
{
	s32 s32_one = 1;

	{
		const u32 ExpectedBitIndex = 3;
		BitWordType data = s32_one << ExpectedBitIndex;
		bitword::foreachSetBit([&](u32 bit) { ASSERT_EQ(bit, ExpectedBitIndex % 32); }, data);
	}
	{
		const u32 ExpectedBitIndex = 30;
		BitWordType data = s32_one << ExpectedBitIndex;
		bitword::foreachSetBit([&](u32 bit) { ASSERT_EQ(bit, ExpectedBitIndex); }, data);
	}
	{
		// I think this is a defined behavior
		const u32 ExpectedBitIndex = 31;
		BitWordType data = s32_one << ExpectedBitIndex;
		BitWordType casted = 0xFFFFFFFF80000000;
		ASSERT_EQ(data, casted);
	}
	//{
	//	// this is UB
	//	const u32 ExpectedBitIndex = 32;
	//	BitWordType data = s32_one << ExpectedBitIndex;
	//	bitword::foreachSetBit([&](u32 bit) { ASSERT_EQ(bit, 0u); }, data);
	//}
	//{
	//	// this is UB
	//	const u32 ExpectedBitIndex = 33;
	//	BitWordType data = s32_one << ExpectedBitIndex;
	//	bitword::foreachSetBit([&](u32 bit) { ASSERT_EQ(bit, ExpectedBitIndex % 32); }, data);
	//}
	//{
	//	// this is UB
	//	const u32 ExpectedBitIndex = 63;
	//	BitWordType data = s32_one << ExpectedBitIndex;
	//	BitWordType casted = 0xFFFFFFFF80000000;
	//	ASSERT_EQ(data, casted);
	//}
}
#pragma warning( pop )

}