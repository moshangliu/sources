#include "ListenThread.h"

#include <gtest/gtest.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>

int main(int argc, char **argv)
{
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}

const uint32 PORT = 80;
class Test_ListenThread_Suite : public testing::Test
{
    protected:
        static void SetUpTestCase()
        {
            thread = new ListenThread(PORT);
        }

        static void TearDownTestCase()
        {
            delete thread;
        }

        static ListenThread* thread;
};

ListenThread* Test_ListenThread_Suite::thread = NULL;

TEST_F(Test_ListenThread_Suite, test_GetSvrAddr)
{
    struct sockaddr_in* svrAddr = new sockaddr_in();

    memset(svrAddr, 0, sizeof(struct sockaddr_in));
    svrAddr->sin_family = AF_INET;
    svrAddr->sin_addr.s_addr = htonl(INADDR_ANY);
    svrAddr->sin_port = htons(PORT);

    struct sockaddr_in* t_svrAddr = thread->GetSvrAddr();

    ASSERT_EQ(svrAddr->sin_family, t_svrAddr->sin_family);
    ASSERT_EQ(svrAddr->sin_addr.s_addr, t_svrAddr->sin_addr.s_addr);
    ASSERT_EQ(svrAddr->sin_port, t_svrAddr->sin_port);

    delete svrAddr;
    delete t_svrAddr;
}
