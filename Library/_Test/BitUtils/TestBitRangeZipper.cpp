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
};

static_assert(sizeof(BitWordType) == 8);

TEST_F(BitRangeZipperFixture, foreachWord_countInvocations)
{
	const u32 NumWords = 10;
	const u32 BitCount = NumWords * BitsInWord;

	BitWordType _lhs[NumWords];
	BitWordType _rhs[NumWords];

	BitRangeZipper zipper(_lhs, _rhs, BitCount);

	u32 wordCounter = 0;
	zipper.foreachWord([&wordCounter](auto a, auto b) { wordCounter++; });

	ASSERT_EQ(wordCounter, NumWords);
}

TEST_F(BitRangeZipperFixture, foreachWord_countInvocationsWithDanglingBits)
{
	const u32 NumWords = 10;
	const u32 BitCount = NumWords * BitsInWord - 17;

	BitWordType _lhs[NumWords];
	BitWordType _rhs[NumWords];

	BitRangeZipper zipper(_lhs, _rhs, BitCount);

	u32 wordCounter = 0;
	zipper.foreachWord([&wordCounter](auto a, auto b) { wordCounter++; });

	ASSERT_EQ(wordCounter, NumWords);
}

TEST_F(BitRangeZipperFixture, foreachWord_paramsProvidedInLinearOrder)
{
	const u32 NumWords = 10;
	const u32 BitCount = NumWords * BitsInWord - 17;

	BitWordType inputA[NumWords];
	BitWordType inputB[NumWords];
	BitWordType outputA[NumWords];
	BitWordType outputB[NumWords];
	BitRangeZipper zipper(inputA, inputB, BitCount);

	meta::iota_container(inputA, 0);
	meta::iota_container(inputB, 1000);

	uint it = 0;
	zipper.foreachWord([&it, &outputA, &outputB](auto a, auto b) { outputA[it] = a; outputB[it] = b; it++; });

	ASSERT_EQ(it, NumWords);

	for (uint i = 0; i < NumWords; ++i)
	{
		ASSERT_EQ(outputA[i], inputA[i]);
		ASSERT_EQ(outputB[i], inputB[i]);
	}
}

//TEST_F(BitRangeZipperFixture, foreachWord_valuesCanBeReferenced)
//{
//	const u32 ExpectedFullWords = 10;
//	const u32 bitCount = 64 * ExpectedFullWords + 1;
//	BitRangeZipper zipper(_lhs, _rhs, bitCount);
//	u32 counter = 0;
//
//	auto eachWordAction = [this, &counter](auto& a, auto& b) { ++counter; a = counter; b = counter + 1000;  };
//	auto danglingWordAction = [this, &counter](auto& a, auto& b, auto mask) { ++counter; a = counter; b = counter + 1000; };
//
//	zipper.foreachWord(eachWordAction, danglingWordAction);
//
//	ASSERT_EQ(counter, ExpectedFullWords + 1);
//
//	for (uint i = 0; i < ExpectedFullWords + 1; ++i)
//	{
//		ASSERT_EQ(_lhs[i], i + 1);
//		ASSERT_EQ(_rhs[i], i + 1001);
//	}
//}
//
//TEST_F(BitRangeZipperFixture, foreachWord_validateZeroSizeBehavior)
//{
//	BitRangeZipper zipper(_lhs, _rhs, 0);
//
//	u32 counter = 0;
//	auto eachWordAction = [this, &counter](auto a, auto b) { ++counter;  };
//
//	zipper.foreachWord(eachWordAction);
//	ASSERT_EQ(counter, 0u);
//}
//
//TEST_F(BitRangeZipperFixture, foreachWord_validateZeroWordSizeAndDanglingBits)
//{
//	BitWordType outputLHS[BitsInWord];
//	BitWordType outputRHS[BitsInWord];
//
//	uint it = 0;
//	for (uint i = 0; i < BitsInWord; ++i)
//	{
//		const BitWordType lhs[1] = { bitword::Ones };
//		const BitWordType rhs[1] = { bitword::Ones };
//		
//		const u32 BitCount = i;
//
//		BitRangeZipper zipper(lhs, rhs, BitCount);
//
//		zipper.foreachWord([&it, &outputLHS, &outputRHS](auto a, auto b) {
//			outputLHS[it] = a;
//			outputRHS[it] = b;
//			it++;
//		});
//
//		ASSERT_EQ(it, i + 1);
//	}
//
//	for (uint i = 0; i < BitsInWord; ++i)
//	{
//		const BitWordType expected = bitword::Ones % bitword::danglingPart(i);
//		ASSERT_EQ(outputLHS[i], expected);
//		ASSERT_EQ(outputRHS[i], expected);
//	}
//}
/*
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

			BitRangeZipper zipper(_lhs, _rhs, bitCount);
			zipper.foreachWord(eachWordAction, danglingWordAction);

			ASSERT_EQ(wordCounter, i);
			ASSERT_EQ(danglingCounter, 1u);
			ASSERT_EQ(providedMask, ExpectedMask);
		}
		{ // 64
			const u32 bitCount = 64 * i + 64;
			//const BitWordType ExpectedMask = (1ull << (64 % 64)) - 1;
			wordCounter = 0, danglingCounter = 0, providedMask = 0;

			BitRangeZipper zipper(_lhs, _rhs, bitCount);
			zipper.foreachWord(eachWordAction, danglingWordAction);

			ASSERT_EQ(wordCounter, i + 1);
			ASSERT_EQ(danglingCounter, 0u);
			//ASSERT_EQ(mask, ExpectedMask);
		}

		{ // 65
			const u32 bitCount = 64 * i + 65;
			const BitWordType ExpectedMask = (1ull << (65 % 64)) - 1;
			wordCounter = 0, danglingCounter = 0, providedMask = 0;

			BitRangeZipper zipper(_lhs, _rhs, bitCount);
			zipper.foreachWord(eachWordAction, danglingWordAction);

			ASSERT_EQ(wordCounter, i + 1);
			ASSERT_EQ(danglingCounter, 1u);
			ASSERT_EQ(providedMask, ExpectedMask);
		}
	}
}*/

}