#include "Common.h"

#include <iostream>
#include<fcntl.h>
#include <sys/stat.h>
#include <gtest/gtest.h>

using namespace std;

int main(int argc, char **argv)
{
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}

const string WHITE_SPACES = " \r\n";
const string EMPTY = "";
TEST(ltrim, test_ltrim)
{
    ASSERT_EQ(ltrim("hello", WHITE_SPACES), "hello");
    ASSERT_EQ(ltrim(" hello", WHITE_SPACES), "hello");
    ASSERT_EQ(ltrim("  hello", WHITE_SPACES), "hello");
    ASSERT_EQ(ltrim("  hello ", WHITE_SPACES), "hello ");
    ASSERT_EQ(ltrim("    ", WHITE_SPACES), EMPTY);
    ASSERT_EQ(ltrim(EMPTY, WHITE_SPACES), EMPTY);
}

TEST(rtrim, test_rtrim)
{
    ASSERT_EQ(rtrim("hello", WHITE_SPACES), "hello");
    ASSERT_EQ(rtrim("hello ", WHITE_SPACES), "hello");
    ASSERT_EQ(rtrim("  hello", WHITE_SPACES), "  hello");
    ASSERT_EQ(rtrim("  hello ", WHITE_SPACES), "  hello");
    ASSERT_EQ(rtrim("    ", WHITE_SPACES), "");
}

TEST(trim, test_trim)
{
    ASSERT_EQ(trim("hello", WHITE_SPACES), "hello");
    ASSERT_EQ(trim("hello ", WHITE_SPACES), "hello");
    ASSERT_EQ(trim("  hello", WHITE_SPACES), "hello");
    ASSERT_EQ(trim("  hello ", WHITE_SPACES), "hello");
    ASSERT_EQ(trim("    ", WHITE_SPACES), "");
}

