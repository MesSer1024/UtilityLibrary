#include <Library/BitUtils/BitSpan.h>

#include <Core/Types.h>
#include <gtest/gtest.h>
#include <Core/Meta/Meta.h>

namespace ddahlkvist
{

static_assert(sizeof(BitWordType) == sizeof(u64));

TEST(BitCountToMaskTest, bitCountToMask_validateCalculations) {
	{
		BitWordType mask = bitCountToMask(0);
		ASSERT_EQ(mask, 0u);
	}

	{
		BitWordType mask = bitCountToMask(1);
		ASSERT_EQ(mask, 1u);
	}

	{
		BitWordType mask = bitCountToMask(2);
		ASSERT_EQ(mask, 0b11);
	}

	{
		BitWordType mask = bitCountToMask(3);
		ASSERT_EQ(mask, 0b111);
	}
	{
		BitWordType mask = bitCountToMask(4);
		ASSERT_EQ(mask, 0xF);
	}

	{
		BitWordType mask = bitCountToMask(6);
		ASSERT_EQ(mask, 0x3f);
	}

	{
		BitWordType mask = bitCountToMask(8);
		ASSERT_EQ(mask, 0xFF);
	}

	{
		BitWordType mask = bitCountToMask(32);
		ASSERT_EQ(mask, 0xFFFFFFFF);
	}

	{
		BitWordType mask = bitCountToMask(33);
		ASSERT_EQ(mask, 0x1FFFFFFFF);
	}

	{
		BitWordType mask = bitCountToMask(63);
		ASSERT_EQ(mask, 0x7FFFFFFFFFFFFFFF);
	}

	{
		BitWordType mask = bitCountToMask(64);
		ASSERT_EQ(mask, ~0ull);
		ASSERT_EQ(mask, 0xFFFFFFFFFFFFFFFF);
	}
}

TEST(BitCountToMask_DeathTest, bitCountToMask_triggerAssert) {
	ASSERT_DEBUG_DEATH({ int bitCount = 65; bitCountToMask(bitCount); }, "Assertion failed.*");
}

class BitSpanFixture : public testing::Test {
public:
protected:
	void SetUp() override {
		meta::fill_container(_buffer, 0xBEBE0000);
	}

	void TearDown() override {
	}

	u64 _buffer[100];
};

TEST_F(BitSpanFixture, clearBits_rangeIsZeroed) {
    const u32 numWords = 7;
    const u32 bitCount = 64 * numWords;

    BitSpan span(_buffer, bitCount);
    span.clearBits();

    for (uint i = 0; i < numWords; ++i)
        ASSERT_EQ(_buffer[i], BitWordType{ 0 });

	ASSERT_NE(_buffer[numWords + 1], BitWordType{ 0 });
}

TEST_F(BitSpanFixture, setBits_rangeContainOnes) {
	const u32 numWords = 7;
	const u32 bitCount = 64 * numWords;

	BitSpan span(_buffer, bitCount);
	span.setBits();

	for (uint i = 0; i < numWords; ++i)
		ASSERT_EQ(_buffer[i], BitWordType{ ~0ull });

	ASSERT_NE(_buffer[numWords + 1], BitWordType{ ~0ull });
}

TEST_F(BitSpanFixture, danglingBits_handledBySetAndClear) {
	const u32 numWords = 7;
	const u32 bitCount = 64 * numWords + 37;
	const BitWordType danglingMask = (static_cast<BitWordType>(1) << 37) - 1;
	BitWordType Default = 0xBEBEBEBEBEBEBEBE;

	BitSpan span(_buffer, bitCount);

	{
		BitWordType zero = BitWordType{ 0 };
		_buffer[numWords] = Default;
		span.clearBits();

		for (uint i = 0; i < numWords; ++i)
			ASSERT_EQ(_buffer[i], zero);

		auto dangling = _buffer[numWords];
		auto expected = (Default & ~danglingMask);
		ASSERT_EQ(dangling, expected);
	}
	{
		BitWordType Ones = BitWordType{ ~0ull };
		_buffer[numWords] = Default;
		span.setBits();

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
	span.setBits();
	span.clearBits();

	ASSERT_EQ(_buffer[0], Default);
	ASSERT_EQ(_buffer[1], Default);
}

TEST_F(BitSpanFixture, only_14_bits) {
	const BitWordType Default = 0xBEBEBEBEBEBEBEBE;
	meta::fill_container(_buffer, Default);
	
	BitSpan span(_buffer, 14);
	const BitWordType danglingMask = (static_cast<BitWordType>(1) << 14) - 1;

	span.setBits();
	span.clearBits();

	ASSERT_EQ(_buffer[0], (Default &~danglingMask));
	ASSERT_EQ(_buffer[1], Default);
}

TEST_F(BitSpanFixture, tresholds_bitsAroundWordSize) {
	const BitWordType Default = 0xFBFBFBFBFBFBFBFB;
	meta::fill_container(_buffer, Default);

	{
		BitSpan span(_buffer, 63);
		const BitWordType danglingMask = (static_cast<BitWordType>(1) << 63) - 1;
		span.setBits();
		span.clearBits();

		ASSERT_EQ(_buffer[0], (Default & ~danglingMask));
		ASSERT_EQ(_buffer[1], Default);
		ASSERT_EQ(_buffer[2], Default);
	}
	{
		BitSpan span(_buffer, 64);
		span.setBits();
		span.clearBits();

		ASSERT_EQ(_buffer[0], 0u);
		ASSERT_EQ(_buffer[1], Default);
		ASSERT_EQ(_buffer[2], Default);
	}
	{
		BitSpan span(_buffer, 65);
		const BitWordType danglingMask = (static_cast<BitWordType>(1) << 1) - 1;
		span.setBits();
		span.clearBits();

		ASSERT_EQ(_buffer[0], 0u);
		ASSERT_EQ(_buffer[1], (Default & ~danglingMask));
		ASSERT_EQ(_buffer[2], Default);
	}
	{
		BitSpan span(_buffer, 127);
		const BitWordType danglingMask = (static_cast<BitWordType>(1) << 63) - 1;
		span.setBits();
		span.clearBits();

		ASSERT_EQ(_buffer[0], 0u);
		ASSERT_EQ(_buffer[1], (Default & ~danglingMask));
		ASSERT_EQ(_buffer[2], Default);
		ASSERT_EQ(_buffer[3], Default);
	}
	{
		BitSpan span(_buffer, 128);
		span.setBits();
		span.clearBits();

		ASSERT_EQ(_buffer[0], 0u);
		ASSERT_EQ(_buffer[1], 0u);
		ASSERT_EQ(_buffer[2], Default);
		ASSERT_EQ(_buffer[3], Default);
	}
	{
		BitSpan span(_buffer, 129);
		const BitWordType danglingMask = (static_cast<BitWordType>(1) << 1) - 1;
		span.setBits();
		span.clearBits();

		ASSERT_EQ(_buffer[0], 0u);
		ASSERT_EQ(_buffer[1], 0u);
		ASSERT_EQ(_buffer[2], (Default & ~danglingMask));
		ASSERT_EQ(_buffer[3], Default);
	}
}

TEST_F(BitSpanFixture, largeSpan) {
	const BitWordType Default = 0xFBFBFBFBFBFBFBFB;
	meta::fill_container(_buffer, Default);

	{
		BitSpan span(_buffer, 100 * 64);
		const BitWordType danglingMask = (static_cast<BitWordType>(1) << 63) - 1;
		
		span.setBits();
		for (auto value : _buffer)
			ASSERT_EQ(value, BitWordType{ ~0ull });

		span.clearBits();
		for (auto value : _buffer)
			ASSERT_EQ(value, BitWordType{ 0 });
	}
}

}

