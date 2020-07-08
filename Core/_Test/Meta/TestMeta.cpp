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
        ASSERT_EQ(values[i], WordValue);
}

TEST(fill_container_tests, vectorValidation) {
	const u32 NumWords = 13;
	const u64 WordValue = 7;

	std::vector<u64> values(NumWords);

	meta::fill_container(values, WordValue);

	ASSERT_EQ(values.size(), NumWords);

	for (auto v : values)
		ASSERT_EQ(v, WordValue);
}

TEST(iota_container_tests, arrayValidation) {
	const u32 NumWords = 13;
	const u64 WordValue = 7;

	u64 values[NumWords];

	meta::iota_container(values, WordValue);

	for (uint i = 0; i < NumWords; ++i)
		ASSERT_EQ(values[i], WordValue + i);
}

TEST(iota_container_tests, vectorValidation) {
	const u32 NumWords = 13;
	const int WordValue = -7;

	std::vector<int> values(NumWords);

	meta::iota_container(values, WordValue);

	for (int i = 0; i < NumWords; ++i)
		ASSERT_EQ(values[i], WordValue + i);
}


}

