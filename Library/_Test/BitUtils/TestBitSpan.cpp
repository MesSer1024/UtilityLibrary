// copyright Daniel Dahlkvist (c) 2020
#include <Library/BitUtils/BitSpan.h>

#include <Core/Meta/Meta.h>
#include <Core/Types.h>
#include <gtest/gtest.h>

namespace ddahlkvist
{

static_assert(sizeof(BitWordType) == sizeof(u64));

TEST(BitCountToMaskTest, danglingMaskFromBitCount_validateCalculations) {
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

class BitSpanFixture : public testing::Test {
public:
protected:
	void SetUp() override {
	}

	void TearDown() override {
	}

};

TEST_F(BitSpanFixture, clearAll_rangeIsZeroed) {
	const u32 NumWords = 7;
	const u32 BitCount = NumBitsInWord * NumWords;
	BitWordType buffer[NumWords];

	BitSpan span(buffer, BitCount);
	span.clearAll();

	for (uint i = 0; i < NumWords; ++i)
		ASSERT_EQ(buffer[i], bitword::Zero);

	ASSERT_NE(buffer[NumWords + 1], BitWordType{ 0 });
}

TEST_F(BitSpanFixture, setAll_rangeContainOnes) {
	const u32 NumWords = 7;
	const u32 BitCount = NumBitsInWord * NumWords;
	BitWordType buffer[NumWords];

	BitSpan span(buffer, BitCount);
	span.setAll();

	for (uint i = 0; i < NumWords; ++i)
		ASSERT_EQ(buffer[i], bitword::Ones);

	ASSERT_NE(buffer[NumWords + 1], bitword::Ones);
}

TEST_F(BitSpanFixture, danglingBits_handledBySetAndClear) {
	const u32 NumWords = 7;
	const u32 BitCount = NumBitsInWord * NumWords - 37;
	const BitWordType danglingMask = bitword::getDanglingPart(BitCount);
	const BitWordType Default = 0xBEBEBEBEBEBEBEBE;

	BitWordType buffer[NumWords];
	BitSpan span(buffer, BitCount);

	{
		meta::fill_container(buffer, Default);
		span.clearAll();

		// dangling part is supposed to be cleared
		for (uint i = 0; i < NumWords; ++i)
			ASSERT_EQ(buffer[i], bitword::Zero);
	}
	{
		meta::fill_container(buffer, Default);

		span.setAll();

		// dangling part is supposed to be cleared [check all but last word]
		for (uint i = 0; i < NumWords - 1; ++i)
			ASSERT_EQ(buffer[i], bitword::Ones);

		auto dangling = buffer[NumWords - 1];
		auto expected = (bitword::Ones & danglingMask);
		ASSERT_EQ(dangling, expected);
	}
}

TEST_F(BitSpanFixture, canHandleZeroBitCount) {
	const BitWordType Default = 0xBEBEBEBEBEBEBEBE;
	BitWordType buffer[4];

	meta::fill_container(buffer, Default);

	BitSpan span(buffer, 0);
	span.setAll();
	span.clearAll();

	ASSERT_EQ(buffer[0], Default);
	ASSERT_EQ(buffer[1], Default);
}

TEST_F(BitSpanFixture, zeroFullWords_only14bits) {
	const BitWordType Default = 0xBEBEBEBEBEBEBEBE;
	BitWordType buffer[4];
	meta::fill_container(buffer, Default);

	BitSpan span(buffer, 14);
	const BitWordType danglingMask = bitword::getDanglingPart(14);

	span.setAll();
	span.clearAll();

	ASSERT_EQ(buffer[0], bitword::Zero);
	ASSERT_EQ(buffer[1], Default);
}

TEST_F(BitSpanFixture, tresholds_clearValidateBehaviorAroundWordSize) {
	const BitWordType Default = 0xFBFBFBFBFBFBFBFB;
	BitWordType buffer[4];

	{
		const u32 BitCount = 1 * NumBitsInWord - 1;
		BitSpan span(buffer, BitCount);
		
		meta::fill_container(buffer, Default);

		span.clearAll();
		ASSERT_EQ(buffer[0], bitword::Zero);
		ASSERT_EQ(buffer[1], Default);
		ASSERT_EQ(buffer[2], Default);
	}
	{
		const u32 BitCount = 1 * NumBitsInWord;
		BitSpan span(buffer, BitCount);

		meta::fill_container(buffer, Default);

		span.clearAll();
		ASSERT_EQ(buffer[0], bitword::Zero);
		ASSERT_EQ(buffer[1], Default);
		ASSERT_EQ(buffer[2], Default);
	}
	{
		const u32 BitCount = 1 * NumBitsInWord + 1;
		BitSpan span(buffer, BitCount);

		meta::fill_container(buffer, Default);

		span.clearAll();
		ASSERT_EQ(buffer[0], bitword::Zero);
		ASSERT_EQ(buffer[1], bitword::Zero);
		ASSERT_EQ(buffer[2], Default);
	}

	{
		const u32 BitCount = 2 * NumBitsInWord - 1;
		BitSpan span(buffer, BitCount);

		meta::fill_container(buffer, Default);

		span.clearAll();
		ASSERT_EQ(buffer[0], bitword::Zero);
		ASSERT_EQ(buffer[1], bitword::Zero);
		ASSERT_EQ(buffer[2], Default);
	}
	{
		const u32 BitCount = 2 * NumBitsInWord;
		BitSpan span(buffer, BitCount);

		meta::fill_container(buffer, Default);

		span.clearAll();
		ASSERT_EQ(buffer[0], bitword::Zero);
		ASSERT_EQ(buffer[1], bitword::Zero);
		ASSERT_EQ(buffer[2], Default);
	}
	{
		const u32 BitCount = 2 * NumBitsInWord + 1;
		BitSpan span(buffer, BitCount);

		meta::fill_container(buffer, Default);

		span.clearAll();
		ASSERT_EQ(buffer[0], bitword::Zero);
		ASSERT_EQ(buffer[1], bitword::Zero);
		ASSERT_EQ(buffer[2], bitword::Zero);
	}
}

TEST_F(BitSpanFixture, functionality_canHandleLargeSpan) {
	const u32 NumWords = 8591;
	const u32 NumBits = NumWords * NumBitsInWord - 13;
	const BitWordType Default = 0xFBFBFBFBFBFBFBFB;
	
	BitWordType largeBuffer[NumWords];
	
	meta::fill_container(largeBuffer, Default);

	BitSpan span(largeBuffer, NumBits);
	
	{
		span.setAll();

		for (uint i = 0; i < NumWords - 1; ++i)
			ASSERT_EQ(largeBuffer[i], bitword::Ones);

		ASSERT_EQ(largeBuffer[NumWords - 1], bitword::Ones & bitword::getDanglingPart(NumBits));
	}

	{
		span.clearAll();
		for (auto value : largeBuffer)
			ASSERT_EQ(value, bitword::Zero);
	}
}

TEST_F(BitSpanFixture, foreachWord_invokedCorrectly) {
	const u32 NumWords = 30;
	const u32 BitCount = NumWords * NumBitsInWord - 1;

	BitWordType input[NumWords];
	BitWordType output[NumWords];

	meta::iota_container(input, BitWordType{ ~0ull - 491 });

	BitSpan span(input, BitCount);
	
	uint it = 0;
	span.foreachWord([&it, &output](auto a) { output[it++] = a; });

	ASSERT_EQ(it, NumWords);

	for (uint i = 0; i < NumWords; ++i)
		ASSERT_EQ(output[i], input[i]);
}

TEST_F(BitSpanFixture, operator_OREQ) {
	const u32 NumWords = 15;
	const u32 NumBits = NumWords * NumBitsInWord - 17;
	BitWordType lhs[NumWords];
	BitWordType rhs[NumWords];

	const BitWordType DefaultLHS = 0xFF;
	meta::fill_container(lhs, DefaultLHS);
	meta::fill_container(rhs, BitWordType{ 0xbebebebe });

	BitSpan lhsSpan(lhs, NumBits);
	BitSpan rhsSpan(rhs, NumBits);

	lhsSpan |= rhsSpan;

	for (uint i = 0; i < NumWords; ++i)
		ASSERT_EQ(lhs[i], DefaultLHS | rhs[i]);
}

TEST_F(BitSpanFixture, operator_ANDEQ) {
	const u32 NumWords = 15;
	const u32 NumBits = NumWords * NumBitsInWord - 17;
	BitWordType lhs[NumWords];
	BitWordType rhs[NumWords];

	const BitWordType DefaultLHS = 0xFF;
	meta::fill_container(lhs, DefaultLHS);
	meta::fill_container(rhs, BitWordType{ 0xbebebebe });

	BitSpan lhsSpan(lhs, NumBits);
	BitSpan rhsSpan(rhs, NumBits);

	lhsSpan &= rhsSpan;

	for (uint i = 0; i < NumWords; ++i)
		ASSERT_EQ(lhs[i], DefaultLHS & rhs[i]);
}

TEST_F(BitSpanFixture, operator_XOREQ) {
	const u32 NumWords = 15;
	const u32 NumBits = NumWords * NumBitsInWord - 17;
	BitWordType lhs[NumWords];
	BitWordType rhs[NumWords];

	const BitWordType DefaultLHS = 0xFF;
	meta::fill_container(lhs, DefaultLHS);
	meta::fill_container(rhs, BitWordType{ 0xbebebebe });

	BitSpan lhsSpan(lhs, NumBits);
	BitSpan rhsSpan(rhs, NumBits);

	lhsSpan ^= rhsSpan;

	for (uint i = 0; i < NumWords; ++i)
		ASSERT_EQ(lhs[i], DefaultLHS ^ rhs[i]);
}

TEST_F(BitSpanFixture, operators_rhsIsUnmodified) {
	const u32 NumWords = 15;
	const u32 NumBits = NumWords * NumBitsInWord - 17;

	BitWordType lhsBuffer[NumWords];
	BitWordType rhsBuffer[NumWords];
	BitWordType rhsBufferCopy[NumWords];

	const BitWordType DefaultLHS = 0xFF;
	meta::fill_container(lhsBuffer, DefaultLHS);
	meta::fill_container(rhsBuffer, BitWordType{ 0xbebebebe });

	BitSpan lhs(lhsBuffer, NumBits);
	BitSpan rhs(rhsBuffer, NumBits);

	memcpy(rhsBufferCopy, rhsBuffer, sizeof(rhsBuffer)); // also copy eventual cleared bits from RHS

	lhs |= rhs;
	lhs &= rhs;
	lhs ^= rhs;
	//lhs = rhs;

	ASSERT_EQ(memcmp(rhsBufferCopy, rhsBuffer, sizeof(rhsBuffer)), 0);
}

TEST_F(BitSpanFixture, operatorEQ) {
	const u32 NumWords = 15;
	const u32 NumBits = NumWords * NumBitsInWord - 17;

	BitWordType lhsBuffer[NumWords + 1];
	BitWordType rhsBuffer[NumWords + 1];

	BitSpan lhs(lhsBuffer, NumBits);
	BitSpan rhs(rhsBuffer, NumBits);

	{
		meta::fill_container(lhsBuffer, BitWordType{ 0xFF });
		meta::fill_container(rhsBuffer, BitWordType{ 0xbebebebe });

		ASSERT_FALSE(lhs == rhs);
	}
	{
		meta::fill_container(lhsBuffer, BitWordType{ 0xEAEA });
		meta::fill_container(rhsBuffer, BitWordType{ 0xEAEA });

		ASSERT_TRUE(lhs == rhs);
	}
	{
		// this entire word is outside of span
		lhsBuffer[NumWords] = bitword::Ones;
		rhsBuffer[NumWords] = 12313;

		ASSERT_TRUE(lhs == rhs);
	}
	{
		// parts of this word is outside of span
		lhsBuffer[NumWords - 1] = bitword::Ones;
		rhsBuffer[NumWords - 1] = 12313;

		ASSERT_FALSE(lhs == rhs);
	}
	{
		// parts of this word is shared and part is unshared, validate that it only looks at the part that is shared
		const BitWordType DanglingMask = bitword::getDanglingPart(NumBits);
		const BitWordType EqualPart = 0xBEBEBEBEBEBEBEBE;

		lhsBuffer[NumWords - 1] = (EqualPart & DanglingMask) | (bitword::Ones & ~DanglingMask);
		rhsBuffer[NumWords - 1] = (EqualPart & DanglingMask) | (0xabcdabcdabcdabcd & ~DanglingMask);

		ASSERT_TRUE(lhs == rhs);
	}
}

TEST_F(BitSpanFixture, foreachSetBit_invokedWithCorrectBitIndex)
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

TEST_F(BitSpanFixture, foreachSetBit_tresholds)
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

TEST_F(BitSpanFixture, foreachSetBit_withReportedIndexOffset)
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

TEST_F(BitSpanFixture, foreachSetBit_withReportedIndexOffsetCanBeReallyBig)
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

#pragma warning( push )
#pragma warning( disable : 4293 ) // warning C4293: '<<': shift count negative or too big, undefined behavior

//TEST_F(BitSpanFixture, foreachSetBit_shiftTooLarge_ub)
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

TEST_F(BitSpanFixture, foreachSetBit_weirdCastBehavior_andUB)
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

TEST_F(BitSpanFixture, foreachSetBit_correctIndexProvided)
{
	BitWordType buffer[177] = {};
	BitSpan span(buffer, 5279);

	const u32 ExpectedBit1 = 2379;
	const u32 ExpectedBit2 = 5001;

	buffer[ExpectedBit1 / NumBitsInWord] = BitWordType{ 1 } << (ExpectedBit1 % NumBitsInWord);
	buffer[ExpectedBit2 / NumBitsInWord] = BitWordType{ 1 } << (ExpectedBit2 % NumBitsInWord);

	u32 counter = 0;
	u32 indexes[10];
	span.foreachSetBit([&counter, &indexes](u32 bitIdx) { indexes[counter++] = bitIdx; });

	ASSERT_EQ(counter, 2u);
	ASSERT_EQ(indexes[0], ExpectedBit1);
	ASSERT_EQ(indexes[1], ExpectedBit2);
}

TEST_F(BitSpanFixture, foreachSetBit_notInvokedForDanglingBits)
{
	BitWordType buffer[1] = {};
	BitSpan span(buffer, 61);

	buffer[0] |= BitWordType{ 1 } << 59;
	buffer[0] |= BitWordType{ 1 } << 60;
	buffer[0] |= BitWordType{ 1 } << 61;
	buffer[0] |= BitWordType{ 1 } << 62;
	buffer[0] |= BitWordType{ 1 } << 63;

	u32 counter = 0;
	u32 indexes[10];
	span.foreachSetBit([&counter, &indexes](u32 bitIdx) { 
		indexes[counter++] = bitIdx; 
	});

	ASSERT_EQ(counter, 2u);
	ASSERT_EQ(indexes[0], 59u);
	ASSERT_EQ(indexes[1], 60u);
}

TEST_F(BitSpanFixture, foreachSetBit_notInvokedForDanglingBitsNotZeroWord)
{
	BitWordType buffer[4] = {};
	BitSpan span(buffer, 130);

	buffer[0] |= BitWordType{ 1 } << 63;
	buffer[1] |= BitWordType{ 1 } << 63;
	buffer[2] |= 0xffffffff0;

	u32 counter = 0;
	u32 indexes[10];
	span.foreachSetBit([&counter, &indexes](u32 bitIdx) {
		indexes[counter++] = bitIdx;
		});

	ASSERT_EQ(counter, 2u);
	ASSERT_EQ(indexes[0], 63u);
	ASSERT_EQ(indexes[1], 64u + 63);
}

TEST_F(BitSpanFixture, foreachSetBit_testPerformanceManyObjects)
{
	const u32 BitCount = 899101;
	const u32 WordCount = bitword::getNumWordsRequired(BitCount);
	BitWordType buffer[WordCount] = {};
	u32 bitsMarked[WordCount * 8];

	{
		meta::fill_container(buffer, 0x0800080);
		BitSpan span(buffer, BitCount);

		u32 it = 0;
		span.foreachSetBit([&bitsMarked, &it](u32 bitIdx) { bitsMarked[it++] = bitIdx; });
		ASSERT_GT(it, WordCount);
	}

	{
		meta::fill_container(buffer, 0x0020020002);
		BitSpan span(buffer, BitCount);

		u32 it = 0;
		span.foreachSetBit([&bitsMarked, &it](u32 bitIdx) { bitsMarked[it++] = bitIdx; });
		ASSERT_GT(it, WordCount);
	}
}
}

