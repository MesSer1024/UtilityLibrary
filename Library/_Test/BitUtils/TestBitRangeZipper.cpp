// copyright Daniel Dahlkvist (c) 2020
#include <Library/BitUtils/BitSpan.h>

#include <Core/Meta/Meta.h>
#include <Core/Types.h>
#include <gtest/gtest.h>
#include <numeric>

namespace ddahlkvist
{

class BitRangeZipperFixture : public testing::Test {
public:
protected:
	void SetUp() override {
	}

	void TearDown() override {
	}

	BitWordType _bufferA[40];
	BitWordType _bufferB[40];
};

static_assert(sizeof(BitWordType) == 8);

TEST_F(BitRangeZipperFixture, foreachWord_countInvocations)
{
	const u32 ExpectedFullWords = 10;
	const u32 bitCount = 64 * ExpectedFullWords;
	BitRangeZipper zipper(_bufferA, _bufferB, bitCount);

	u32 numFullWords = 0;
	u32 numDangling = 0;
	auto eachWordAction = [&numFullWords](auto a, auto b) { numFullWords++;  };
	auto danglingWordAction = [&numDangling](auto a, auto b, auto mask) {numDangling++;  };
	zipper.foreachWord(eachWordAction, danglingWordAction);

	ASSERT_EQ(numFullWords, ExpectedFullWords);
	ASSERT_EQ(numDangling, 0u);
}

TEST_F(BitRangeZipperFixture, foreachWord_countInvocationsDanglingBits)
{
	const u32 ExpectedFullWords = 10;
	const u32 bitCount = (64 * ExpectedFullWords) + 7;
	BitRangeZipper zipper(_bufferA, _bufferB, bitCount);

	u32 numFullWords = 0;
	u32 numDangling = 0;
	auto eachWordAction = [&numFullWords](auto a, auto b) {numFullWords++;  };
	auto danglingWordAction = [&numDangling](auto a, auto b, auto mask) {numDangling++;  };
	zipper.foreachWord(eachWordAction, danglingWordAction);

	ASSERT_EQ(numFullWords, ExpectedFullWords);
	ASSERT_EQ(numDangling, 1u);
}

TEST_F(BitRangeZipperFixture, foreachWord_paramsProvidedInLinearOrder)
{
	const u32 ExpectedFullWords = 10;
	const u32 bitCount = 64 * ExpectedFullWords + 1;
	BitRangeZipper zipper(_bufferA, _bufferB, bitCount);
	u32 counter = 0;
	BitWordType valuesFromA[ExpectedFullWords + 1];
	BitWordType valuesFromB[ExpectedFullWords + 1];

	meta::iota_container(_bufferA, 0);
	meta::iota_container(_bufferB, 1000);

	auto eachWordAction = [this, &counter, &valuesFromA, &valuesFromB](auto a, auto b) { valuesFromA[counter] = a; valuesFromB[counter] = b; counter++; };
	auto danglingWordAction = [this, &counter, &valuesFromA, &valuesFromB](auto a, auto b, auto mask) { valuesFromA[counter] = a; valuesFromB[counter] = b; counter++; };

	zipper.foreachWord(eachWordAction, danglingWordAction);

	ASSERT_EQ(counter, ExpectedFullWords + 1);

	for (uint i = 0; i < ExpectedFullWords + 1; ++i)
	{
		ASSERT_EQ(valuesFromA[i], _bufferA[i]);
		ASSERT_EQ(valuesFromB[i], _bufferB[i]);
	}
}

TEST_F(BitRangeZipperFixture, foreachWord_valuesCanBeReferenced)
{
	const u32 ExpectedFullWords = 10;
	const u32 bitCount = 64 * ExpectedFullWords + 1;
	BitRangeZipper zipper(_bufferA, _bufferB, bitCount);
	u32 counter = 0;

	auto eachWordAction = [this, &counter](auto& a, auto& b) { ++counter; a = counter; b = counter + 1000;  };
	auto danglingWordAction = [this, &counter](auto& a, auto& b, auto mask) { ++counter; a = counter; b = counter + 1000; };

	zipper.foreachWord(eachWordAction, danglingWordAction);

	ASSERT_EQ(counter, ExpectedFullWords + 1);

	for (uint i = 0; i < ExpectedFullWords + 1; ++i)
	{
		ASSERT_EQ(_bufferA[i], i + 1);
		ASSERT_EQ(_bufferB[i], i + 1001);
	}
}

TEST_F(BitRangeZipperFixture, foreachWord_validateZeroSizeBehavior)
{
	BitRangeZipper zipper(_bufferA, _bufferB, 0);

	u32 counter = 0;
	auto eachWordAction = [this, &counter](auto a, auto b) { ++counter;  };
	auto danglingWordAction = [this, &counter](auto a, auto b, auto mask) { ++counter;  };

	zipper.foreachWord(eachWordAction, danglingWordAction);
	ASSERT_EQ(counter, 0u);
}

TEST_F(BitRangeZipperFixture, foreachWord_validateZeroFullWordBehaviorWithDanglingBits)
{
	BitWordType ExpectedMasks[64];

	for (uint i = 0; i < 64; ++i)
		ExpectedMasks[i] = (1ull << i) - 1;


	for (uint i = 1; i < 64; ++i)
	{
		const u32 bitCount = i;

		BitRangeZipper zipper(_bufferA, _bufferB, bitCount);

		BitWordType providedMask;
		u32 wordCounter = 0;
		u32 danglingCounter = 0;
		auto eachWordAction = [&wordCounter](auto a, auto b) { ++wordCounter;  };
		auto danglingWordAction = [&danglingCounter, &providedMask](auto a, auto b, auto mask) { ++danglingCounter; providedMask = mask;  };

		zipper.foreachWord(eachWordAction, danglingWordAction);

		const BitWordType ExpectedMask = ExpectedMasks[i];
		ASSERT_EQ(wordCounter, 0u);
		ASSERT_EQ(danglingCounter, 1u);
		ASSERT_EQ(providedMask, ExpectedMask);
	}
}

TEST_F(BitRangeZipperFixture, foreachWord_validateTresholdBitCounts)
{
	BitWordType providedMask;
	u32 wordCounter, danglingCounter;
	auto eachWordAction = [&wordCounter](auto a, auto b) { ++wordCounter;  };
	auto danglingWordAction = [&danglingCounter, &providedMask](auto a, auto b, auto mask) { ++danglingCounter; providedMask = mask; };

	for (uint i = 0; i < 20; ++i)
	{
		{ // 63
			const u32 bitCount = 64 * i + 63;
			const BitWordType ExpectedMask = (1ull << (63 % 64)) - 1;
			wordCounter = 0, danglingCounter = 0, providedMask = 0;

			BitRangeZipper zipper(_bufferA, _bufferB, bitCount);
			zipper.foreachWord(eachWordAction, danglingWordAction);

			ASSERT_EQ(wordCounter, i);
			ASSERT_EQ(danglingCounter, 1u);
			ASSERT_EQ(providedMask, ExpectedMask);
		}
		{ // 64
			const u32 bitCount = 64 * i + 64;
			//const BitWordType ExpectedMask = (1ull << (64 % 64)) - 1;
			wordCounter = 0, danglingCounter = 0, providedMask = 0;

			BitRangeZipper zipper(_bufferA, _bufferB, bitCount);
			zipper.foreachWord(eachWordAction, danglingWordAction);

			ASSERT_EQ(wordCounter, i + 1);
			ASSERT_EQ(danglingCounter, 0u);
			//ASSERT_EQ(mask, ExpectedMask);
		}

		{ // 65
			const u32 bitCount = 64 * i + 65;
			const BitWordType ExpectedMask = (1ull << (65 % 64)) - 1;
			wordCounter = 0, danglingCounter = 0, providedMask = 0;

			BitRangeZipper zipper(_bufferA, _bufferB, bitCount);
			zipper.foreachWord(eachWordAction, danglingWordAction);

			ASSERT_EQ(wordCounter, i + 1);
			ASSERT_EQ(danglingCounter, 1u);
			ASSERT_EQ(providedMask, ExpectedMask);
		}
	}
}

}