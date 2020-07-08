// copyright Daniel Dahlkvist (c) 2020
#include <Library/BitUtils/BitSpan.h>

#include <Core/Meta/Meta.h>
#include <Core/Types.h>
#include <gtest/gtest.h>

namespace ddahlkvist
{

static_assert(sizeof(BitWordType) == sizeof(u64));

TEST(BitCountToMaskTest, bitCountToMask_validateCalculations) {
	{
		constexpr BitWordType mask = numBitsToMask(0);
		ASSERT_EQ(mask, BitWordType{ 0u });
	}

	{
		constexpr BitWordType mask = numBitsToMask(1);
		ASSERT_EQ(mask, BitWordType{ 1u });
	}

	{
		constexpr BitWordType mask = numBitsToMask(2);
		ASSERT_EQ(mask, BitWordType{ 0b11 });
	}

	{
		constexpr BitWordType mask = numBitsToMask(3);
		ASSERT_EQ(mask, BitWordType{ 0b111 });
	}
	{
		constexpr BitWordType mask = numBitsToMask(4);
		ASSERT_EQ(mask, BitWordType{ 0xF });
	}

	{
		constexpr BitWordType mask = numBitsToMask(6);
		ASSERT_EQ(mask, BitWordType{ 0x3f });
	}

	{
		constexpr BitWordType mask = numBitsToMask(8);
		ASSERT_EQ(mask, BitWordType{ 0xFF });
	}

	{
		constexpr BitWordType mask = numBitsToMask(32);
		ASSERT_EQ(mask, BitWordType{ 0xFFFFFFFF });
	}

	{
		constexpr BitWordType mask = numBitsToMask(33);
		ASSERT_EQ(mask, BitWordType{ 0x1FFFFFFFF });
	}

	{
		constexpr BitWordType mask = numBitsToMask(63);
		ASSERT_EQ(mask, BitWordType{ 0x7FFFFFFFFFFFFFFF });
	}

	{
		constexpr BitWordType mask = numBitsToMask(64);
		ASSERT_EQ(mask, BitWordType{ ~0ull });
		ASSERT_EQ(mask, BitWordType{ 0xFFFFFFFFFFFFFFFF });
	}
}
TEST(BitCountToMask_DeathTest, bitCountToMask_triggerAssert) {
	ASSERT_DEBUG_DEATH({ int bitCount = 65; numBitsToMask(bitCount); }, "Assertion failed.*");
}

class BitSpanFixture : public testing::Test {
public:
protected:
	void SetUp() override {
		meta::fill_container(_buffer, 0xBEBE0000);
	}

	void TearDown() override {
	}

	BitWordType _buffer[100];
};

TEST_F(BitSpanFixture, clearAll_rangeIsZeroed) {
	const u32 numWords = 7;
	const u32 bitCount = BitsInWord * numWords;

	BitSpan span(_buffer, bitCount);
	span.clearAll();

	for (uint i = 0; i < numWords; ++i)
		ASSERT_EQ(_buffer[i], BitWordType{ 0 });

	ASSERT_NE(_buffer[numWords + 1], BitWordType{ 0 });
}

TEST_F(BitSpanFixture, setAll_rangeContainOnes) {
	const u32 numWords = 7;
	const u32 bitCount = BitsInWord * numWords;

	BitSpan span(_buffer, bitCount);
	span.setAll();

	for (uint i = 0; i < numWords; ++i)
		ASSERT_EQ(_buffer[i], BitWordType{ ~0ull });

	ASSERT_NE(_buffer[numWords + 1], BitWordType{ ~0ull });
}

TEST_F(BitSpanFixture, danglingBits_handledBySetAndClear) {
	const u32 numWords = 7;
	const u32 bitCount = BitsInWord *numWords + 37;
	const BitWordType danglingMask = (static_cast<BitWordType>(1) << 37) - 1;
	BitWordType Default = 0xBEBEBEBEBEBEBEBE;

	BitSpan span(_buffer, bitCount);

	{
		BitWordType zero = BitWordType{ 0 };
		_buffer[numWords] = Default;
		span.clearAll();

		for (uint i = 0; i < numWords; ++i)
			ASSERT_EQ(_buffer[i], zero);

		auto dangling = _buffer[numWords];
		auto expected = (Default & ~danglingMask);
		ASSERT_EQ(dangling, expected);
	}
	{
		BitWordType Ones = BitWordType{ ~0ull };
		_buffer[numWords] = Default;
		span.setAll();

		for (uint i = 0; i < numWords; ++i)
			ASSERT_EQ(_buffer[i], Ones);

		auto dangling = _buffer[numWords];
		auto expected = (Default & ~danglingMask) | danglingMask;
		ASSERT_EQ(dangling, expected);
	}
}

TEST_F(BitSpanFixture, ctor_zeroSize) {
	const BitWordType Default = 0xBEBEBEBEBEBEBEBE;
	meta::fill_container(_buffer, Default);

	BitSpan span(_buffer, 0);
	span.setAll();
	span.clearAll();

	ASSERT_EQ(_buffer[0], Default);
	ASSERT_EQ(_buffer[1], Default);
}

TEST_F(BitSpanFixture, only_14_bits) {
	const BitWordType Default = 0xBEBEBEBEBEBEBEBE;
	meta::fill_container(_buffer, Default);

	BitSpan span(_buffer, 14);
	const BitWordType danglingMask = numBitsToMask(14);

	span.setAll();
	span.clearAll();

	ASSERT_EQ(_buffer[0], (Default & ~danglingMask));
	ASSERT_EQ(_buffer[1], Default);
}

TEST_F(BitSpanFixture, tresholds_bitsAroundWordSize) {
	const BitWordType Default = 0xFBFBFBFBFBFBFBFB;
	meta::fill_container(_buffer, Default);

	{
		BitSpan span(_buffer, 63);
		const BitWordType danglingMask = numBitsToMask(63);
		span.setAll();
		span.clearAll();

		ASSERT_EQ(_buffer[0], (Default & ~danglingMask));
		ASSERT_EQ(_buffer[1], Default);
		ASSERT_EQ(_buffer[2], Default);
	}
	{
		BitSpan span(_buffer, 64);
		span.setAll();
		span.clearAll();

		ASSERT_EQ(_buffer[0], 0u);
		ASSERT_EQ(_buffer[1], Default);
		ASSERT_EQ(_buffer[2], Default);
	}
	{
		BitSpan span(_buffer, 65);
		const BitWordType danglingMask = numBitsToMask(65 % BitsInWord);
		span.setAll();
		span.clearAll();

		ASSERT_EQ(_buffer[0], 0u);
		ASSERT_EQ(_buffer[1], (Default & ~danglingMask));
		ASSERT_EQ(_buffer[2], Default);
	}
	{
		BitSpan span(_buffer, 127);
		const BitWordType danglingMask = numBitsToMask(127 % BitsInWord);
		span.setAll();
		span.clearAll();

		ASSERT_EQ(_buffer[0], 0u);
		ASSERT_EQ(_buffer[1], (Default & ~danglingMask));
		ASSERT_EQ(_buffer[2], Default);
		ASSERT_EQ(_buffer[3], Default);
	}
	{
		BitSpan span(_buffer, 128);
		span.setAll();
		span.clearAll();

		ASSERT_EQ(_buffer[0], 0u);
		ASSERT_EQ(_buffer[1], 0u);
		ASSERT_EQ(_buffer[2], Default);
		ASSERT_EQ(_buffer[3], Default);
	}
	{
		BitSpan span(_buffer, 129);
		const BitWordType danglingMask = numBitsToMask(129 % BitsInWord);
		span.setAll();
		span.clearAll();

		ASSERT_EQ(_buffer[0], 0u);
		ASSERT_EQ(_buffer[1], 0u);
		ASSERT_EQ(_buffer[2], (Default & ~danglingMask));
		ASSERT_EQ(_buffer[3], Default);
	}
}

TEST_F(BitSpanFixture, functionality_canHandleLargeSpan452003bits) {
	const u32 numBits = 452003;
	const u32 LastElement = numBits / BitsInWord;
	BitWordType largeBuffer[LastElement + 1];
	const BitWordType Default = 0xFBFBFBFBFBFBFBFB;
	meta::fill_container(largeBuffer, Default);

	BitSpan span(largeBuffer, numBits);
	span.setAll();
	uint i = 0;
	const BitWordType DanglingOne = (Default & ~numBitsToMask(numBits % BitsInWord)) | (BitWordType{ ~0ull } &numBitsToMask(numBits % BitsInWord));
	const BitWordType DanglingZero = (Default & ~numBitsToMask(numBits % BitsInWord)) | (BitWordType{ 0 } &numBitsToMask(numBits % BitsInWord));
	for (auto value : largeBuffer)
	{
		if (i++ < LastElement)
			ASSERT_EQ(value, BitWordType{ ~0ull });
		else
			ASSERT_EQ(value, DanglingOne);
	}

	i = 0;
	span.clearAll();
	for (auto value : largeBuffer)
		if (i++ < LastElement)
			ASSERT_EQ(value, BitWordType{ 0 });
		else
			ASSERT_EQ(value, DanglingZero);

}

}

