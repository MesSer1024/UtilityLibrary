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
		constexpr BitWordType mask = bitword::danglingPart(0);
		ASSERT_EQ(mask, BitWordType{ 0u });
	}

	{
		constexpr BitWordType mask = bitword::danglingPart(1);
		ASSERT_EQ(mask, BitWordType{ 1u });
	}

	{
		constexpr BitWordType mask = bitword::danglingPart(2);
		ASSERT_EQ(mask, BitWordType{ 0b11 });
	}

	{
		constexpr BitWordType mask = bitword::danglingPart(3);
		ASSERT_EQ(mask, BitWordType{ 0b111 });
	}
	{
		constexpr BitWordType mask = bitword::danglingPart(4);
		ASSERT_EQ(mask, BitWordType{ 0xF });
	}

	{
		constexpr BitWordType mask = bitword::danglingPart(6);
		ASSERT_EQ(mask, BitWordType{ 0x3f });
	}

	{
		constexpr BitWordType mask = bitword::danglingPart(8);
		ASSERT_EQ(mask, BitWordType{ 0xFF });
	}

	{
		constexpr BitWordType mask = bitword::danglingPart(32);
		ASSERT_EQ(mask, BitWordType{ 0xFFFFFFFF });
	}

	{
		constexpr BitWordType mask = bitword::danglingPart(33);
		ASSERT_EQ(mask, BitWordType{ 0x1FFFFFFFF });
	}

	{
		constexpr BitWordType mask = bitword::danglingPart(63);
		ASSERT_EQ(mask, BitWordType{ 0x7FFFFFFFFFFFFFFF });
	}
	{
		constexpr BitWordType mask = bitword::danglingPart(64);
		ASSERT_EQ(mask, BitWordType{ 0 });
	}
	{
		constexpr BitWordType mask = bitword::danglingPart(65);
		ASSERT_EQ(mask, BitWordType{ 1 });
	}
	{
		constexpr BitWordType mask = bitword::danglingPart(130);
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
	const u32 BitCount = BitsInWord * NumWords;
	BitWordType buffer[NumWords];

	BitSpan span(buffer, BitCount);
	span.clearAll();

	for (uint i = 0; i < NumWords; ++i)
		ASSERT_EQ(buffer[i], BitWordType{ 0 });

	ASSERT_NE(buffer[NumWords + 1], BitWordType{ 0 });
}

TEST_F(BitSpanFixture, setAll_rangeContainOnes) {
	const u32 NumWords = 7;
	const u32 BitCount = BitsInWord * NumWords;
	BitWordType buffer[NumWords];

	BitSpan span(buffer, BitCount);
	span.setAll();

	for (uint i = 0; i < NumWords; ++i)
		ASSERT_EQ(buffer[i], BitWordType{ ~0ull });

	ASSERT_NE(buffer[NumWords + 1], BitWordType{ ~0ull });
}

TEST_F(BitSpanFixture, danglingBits_handledBySetAndClear) {
	const u32 NumWords = 7;
	const u32 BitCount = BitsInWord * NumWords - 37;
	const BitWordType danglingMask = bitword::danglingPart(BitCount);
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
	const BitWordType danglingMask = bitword::danglingPart(14);

	span.setAll();
	span.clearAll();

	ASSERT_EQ(buffer[0], bitword::Zero);
	ASSERT_EQ(buffer[1], Default);
}

TEST_F(BitSpanFixture, tresholds_clearValidateBehaviorAroundWordSize) {
	const BitWordType Default = 0xFBFBFBFBFBFBFBFB;
	BitWordType buffer[4];

	{
		const u32 BitCount = 1 * BitsInWord - 1;
		BitSpan span(buffer, BitCount);
		
		meta::fill_container(buffer, Default);

		span.clearAll();
		ASSERT_EQ(buffer[0], bitword::Zero);
		ASSERT_EQ(buffer[1], Default);
		ASSERT_EQ(buffer[2], Default);
	}
	{
		const u32 BitCount = 1 * BitsInWord;
		BitSpan span(buffer, BitCount);

		meta::fill_container(buffer, Default);

		span.clearAll();
		ASSERT_EQ(buffer[0], bitword::Zero);
		ASSERT_EQ(buffer[1], Default);
		ASSERT_EQ(buffer[2], Default);
	}
	{
		const u32 BitCount = 1 * BitsInWord + 1;
		BitSpan span(buffer, BitCount);

		meta::fill_container(buffer, Default);

		span.clearAll();
		ASSERT_EQ(buffer[0], bitword::Zero);
		ASSERT_EQ(buffer[1], bitword::Zero);
		ASSERT_EQ(buffer[2], Default);
	}

	{
		const u32 BitCount = 2 * BitsInWord - 1;
		BitSpan span(buffer, BitCount);

		meta::fill_container(buffer, Default);

		span.clearAll();
		ASSERT_EQ(buffer[0], bitword::Zero);
		ASSERT_EQ(buffer[1], bitword::Zero);
		ASSERT_EQ(buffer[2], Default);
	}
	{
		const u32 BitCount = 2 * BitsInWord;
		BitSpan span(buffer, BitCount);

		meta::fill_container(buffer, Default);

		span.clearAll();
		ASSERT_EQ(buffer[0], bitword::Zero);
		ASSERT_EQ(buffer[1], bitword::Zero);
		ASSERT_EQ(buffer[2], Default);
	}
	{
		const u32 BitCount = 2 * BitsInWord + 1;
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
	const u32 NumBits = NumWords * BitsInWord - 13;
	const BitWordType Default = 0xFBFBFBFBFBFBFBFB;
	
	BitWordType largeBuffer[NumWords];
	
	meta::fill_container(largeBuffer, Default);

	BitSpan span(largeBuffer, NumBits);
	
	{
		span.setAll();

		for (uint i = 0; i < NumWords - 1; ++i)
			ASSERT_EQ(largeBuffer[i], bitword::Ones);

		ASSERT_EQ(largeBuffer[NumWords - 1], bitword::Ones & bitword::danglingPart(NumBits));
	}

	{
		span.clearAll();
		for (auto value : largeBuffer)
			ASSERT_EQ(value, bitword::Zero);
	}
}

TEST_F(BitSpanFixture, foreachWord_invokedCorrectly) {
	const u32 NumWords = 30;
	const u32 BitCount = NumWords * BitsInWord - 1;

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
	const u32 NumBits = NumWords * BitsInWord - 17;
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
	const u32 NumBits = NumWords * BitsInWord - 17;
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
	const u32 NumBits = NumWords * BitsInWord - 17;
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

//
//
//TEST_F(BitSpanFixture, operatorEQ_pointersRemains) {
//}



}

