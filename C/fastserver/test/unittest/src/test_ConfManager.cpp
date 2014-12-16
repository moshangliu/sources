#include "Common.h"
#include "ConfManager.h"

#include <gtest/gtest.h>

int main(int argc, char **argv)
{
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}

class TEST_ConfManager_Suite : public testing::Test
{
    public:
        static void SetUpTestCase()
        {

        }

        static void TearDownTestCase()
        {

        }
};

TEST_F(TEST_ConfManager_Suite, test_ConfManager_Right)
{
    ASSERT_EQ(ConfManager::Instance()->GetPort(), 8080);
    ASSERT_EQ(ConfManager::Instance()->GetMaxConnCnt(), 1024);
}
