#include <Core/Meta/Meta.h>
#include <Core/Types.h>
#include <gtest/gtest.h>
#include <vector>

namespace ddahlkvist
{

TEST(fill_container_tests, arrayValidation) {
    const u32 NumWords = 13;
    const u64 WordValue = 7;

    u64 values[NumWords];

    meta::fill_container(values, WordValue);

    for (uint i = 0; i < NumWords; ++i)
        EXPECT_EQ(values[i], WordValue);
}

TEST(fill_container_tests, vectorValidation) {
	const u32 NumWords = 13;
	const u64 WordValue = 7;

	std::vector<u64> values(NumWords);

	meta::fill_container(values, WordValue);

	EXPECT_EQ(values.size(), NumWords);

	for (auto v : values)
		EXPECT_EQ(v, WordValue);
}

//TEST(fill_container_tests, spanValidation) {
//	const u32 NumWords = 13;
//	const u64 WordValue = 7;
//
//	std::vector<u64> values(NumWords);
//	std::span<u64> valueSpan(values);
//
//	meta::fill_container(valueSpan, WordValue);
//
//	EXPECT_EQ(values.size(), NumWords);
//
//	for (auto v : values)
//		EXPECT_EQ(v, WordValue);
//}

//TEST(fill_container_tests, notOKInitializerList) {
//	const u32 NumWords = 13;
//	const u64 WordValue = 7;
//
//	meta::fill_container({ 13,1,14 }, WordValue);
//}



}

