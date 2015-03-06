#include "SafeMap.h"

#include <string>
#include <gtest/gtest.h>

using namespace std;

int main(int argc, char **argv)
{
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}

TEST(SafeMap, test_SafeMap) {
    SafeMap<string, int> data;
    ASSERT_EQ(0, data.size());

    data.put("K1", 1);
    ASSERT_EQ(1, data.size());
    data.put("K2", 2);
    ASSERT_EQ(2, data.size());

    ASSERT_TRUE(data.has("K1"));
    ASSERT_TRUE(data.has("K2"));
    ASSERT_FALSE(data.has("K3"));

    data.put("K3", 3);
    ASSERT_EQ(3, data.size());
    ASSERT_TRUE(data.has("K3"));

    ASSERT_EQ(1, data.get("K1"));
    ASSERT_EQ(2, data.get("K2"));
    ASSERT_EQ(3, data.get("K3"));

    data.erase("K3");
    ASSERT_EQ(2, data.size());
    ASSERT_FALSE(data.has("K3"));

    data.erase("K4");
    ASSERT_EQ(2, data.size());
    ASSERT_FALSE(data.has("K3"));
}
