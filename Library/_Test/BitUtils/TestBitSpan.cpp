#include <Library/BitUtils/BitSpan.h>

#include <Core/Types.h>
#include <gtest/gtest.h>
#include <Core/Meta/Meta.h>

namespace ddahlkvist
{

constexpr u32 WordCount = 64;

class BitSpanFixture : public testing::Test {
public:
protected:
    void SetUp() override { 
		meta::fill_container(_buffer, 0);
    }
    void TearDown() override { 
    }

    u64 _buffer[WordCount];
};

TEST_F(BitSpanFixture, read_initializedToZero) {
    EXPECT_EQ(_buffer[3], 0u);
}

}

