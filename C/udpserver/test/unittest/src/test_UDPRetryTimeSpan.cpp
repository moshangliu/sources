#include "UDPRetryTimeSpan.h"

#include <gtest/gtest.h>

int main(int argc, char **argv)
{
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}

TEST(UDPRetryTimeSpan, test_maxTryCnt) {
    ASSERT_EQ(6, UDPRetryTimeSpan::instance()->maxTryCnt());
}

TEST(UDPRetryTimeSpan, test_allWaitTimeUs) {
    int expectedAllWaitTimeUs = ZERO_WAIT_TIME_US
        + ONE_WAIT_TIME_US
        + TWO_WAIT_TIME_US
        + THREE_WAIT_TIME_US
        + FOUR_WAIT_TIME_US
        + FIVE_WAIT_TIME_US
        + SIX_WAIT_TIME_US;
    ASSERT_EQ(expectedAllWaitTimeUs, UDPRetryTimeSpan::instance()->allWaitTimeUs());
}

TEST(UDPRetryTimeSpan, test_waitTimeUs) {
    ASSERT_EQ(ZERO_WAIT_TIME_US, UDPRetryTimeSpan::instance()->waitTimeUs(0));
    ASSERT_EQ(ONE_WAIT_TIME_US, UDPRetryTimeSpan::instance()->waitTimeUs(1));
    ASSERT_EQ(TWO_WAIT_TIME_US, UDPRetryTimeSpan::instance()->waitTimeUs(2));
    ASSERT_EQ(THREE_WAIT_TIME_US, UDPRetryTimeSpan::instance()->waitTimeUs(3));
    ASSERT_EQ(FOUR_WAIT_TIME_US, UDPRetryTimeSpan::instance()->waitTimeUs(4));
    ASSERT_EQ(FIVE_WAIT_TIME_US, UDPRetryTimeSpan::instance()->waitTimeUs(5));
    ASSERT_EQ(SIX_WAIT_TIME_US, UDPRetryTimeSpan::instance()->waitTimeUs(6));
}
