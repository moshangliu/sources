#include "Common.h"
#include "HttpResponse.h"

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

const int BUF_SIZE = 1024;
TEST(readn, test_readn_invalid_fd)
{
    char buf[BUF_SIZE];
    memset(buf, 0, BUF_SIZE);
    int ret = readn(-1, buf, BUF_SIZE);

    ASSERT_EQ(ret, -1);
}

TEST(readn, test_readn_buf_null)
{
    int fd = open("../data/TestData_ReadN", O_RDONLY);
    int ret = readn(fd, NULL, BUF_SIZE);
    close(fd);

    ASSERT_EQ(ret, -1);
}

TEST(readn, test_readn_equal_filesize)
{
    int fd = open("../data/TestData_ReadN_10", O_RDONLY);
    const int nbytes = 10;
    byte buf[nbytes + 1];
    memset(buf, 0, nbytes + 1);

    int nread = readn(fd, (char*)buf, nbytes);
    close(fd);

    ASSERT_EQ(nread, nbytes) << "nread = " << nread << "; nbytes = " << nbytes;
}

TEST(readn, test_readn_more_than_filesize)
{
    int fd = open("../data/TestData_ReadN_10", O_RDONLY);
    const int nbytes = 20;
    char buf[nbytes + 1];
    memset(buf, 0, nbytes + 1);

    int nread = readn(fd, buf, nbytes);
    close(fd);

    ASSERT_EQ(nread, 10) << nread;
}

TEST(writen, test_writen_buf_null)
{
    int fd = open("../data/TestData_WriteN", O_WRONLY);
    int nwrite = writen(fd, NULL, 10);
    close(fd);

    ASSERT_EQ(nwrite, -1);
}

TEST(writen, test_writen)
{
    int fd = open("../data/TestData_WriteN_2", O_WRONLY);
    const char* content = "TestData_WriteN_TestCase_Content";
    int nwrite = writen(fd, content, strlen(content));
    close(fd);
    ASSERT_EQ(nwrite, strlen(content));
}

TEST(writen, test_writen_readn)
{
    int fd = open("../data/TestData_WriteN_2", O_WRONLY | O_CREAT);
    const char* content = "TestData_WriteN_TestCase_Content";
    int nwrite = writen(fd, content, strlen(content));
    close(fd);
    ASSERT_EQ(nwrite, strlen(content));

    fd = open("../data/TestData_WriteN_2", O_RDONLY);
    char* buf = new char[nwrite + 1];
    memset(buf, 0, nwrite + 1);
    int nread = readn(fd, buf, nwrite);
    close(fd);
    ASSERT_EQ(nwrite, nread);

    ASSERT_STREQ(content, buf);
}

TEST(readLine, test_readLine_invalid_fd)
{
    char* buf = CreateCStr(BUF_SIZE);
    ASSERT_EQ(readLine(-1, buf, BUF_SIZE), -1);
    delete [] buf;
}

TEST(readLine, test_readLine_buf_null)
{
    int fd = open("../data/TestData_ReadLine", O_RDONLY);
    ASSERT_GE(fd, 0);
    ASSERT_EQ(readLine(fd, NULL, BUF_SIZE), -1);
    close(fd);
}

TEST(readLine, test_readLine_buf_size_zero)
{
    char* buf = CreateCStr(BUF_SIZE);
    int fd = open("../data/TestData_ReadLine", O_RDONLY);
    ASSERT_GE(fd, 0);
    ASSERT_EQ(readLine(fd, buf, 0), -1);
    delete [] buf;
    close(fd);
}

TEST(readLine, test_readLine_right)
{
    const char* content = "PORT = 8080\r\n";

    char* buf = CreateCStr(BUF_SIZE);
    int fd = open("../data/TestData_ReadLine", O_RDONLY);
    ASSERT_GE(fd, 0);
    ASSERT_EQ(readLine(fd, buf, BUF_SIZE), strlen(content));
    ASSERT_STREQ(buf, content);
    delete [] buf;
    close(fd);
}

class Test_Split_Suite : public testing::Test
{
    protected:
        static void SetUpTestCase()
        {
            str = "GET / HTTP/1.0\r\n"
                "User-Agent: Mozilla/4.0 (compatible; MSIE 6.0;\r\n"
                "Host: www.baidu.com\r\n"
                "\r\n";

            expected.push_back("GET / HTTP/1.0");
            expected.push_back("User-Agent: Mozilla/4.0 (compatible; MSIE 6.0;\r\n");
            expected.push_back("Host: www.baidu.com");
            expected.push_back("");
        }

        static void TearDownTestCase() { }

        static string str;
        static vector<string> expected;
};

string Test_Split_Suite::str = "";
vector<string> Test_Split_Suite::expected = vector<string>();
TEST_F(Test_Split_Suite, test_Split_delim_empty)
{
    vector<string> parts;
    int ret = Split(str, "", parts);

    ASSERT_EQ(ret, -1);
    ASSERT_EQ(parts.size(), 0);
}

TEST_F(Test_Split_Suite, test_Split_delim_not_empty)
{
    vector<string> parts;
    int ret = Split(str, "\r\n", parts);

    ASSERT_EQ(ret, 0);
    ASSERT_EQ(parts.size(), 4);
}

TEST_F(Test_Split_Suite, test_Split_delim_not_in)
{
    vector<string> parts;
    int ret = Split(str, "DELIM_NOT_IN", parts);

    ASSERT_EQ(ret, 0);
    ASSERT_EQ(parts.size(), 1);
    ASSERT_EQ(parts[0], str);
}

TEST(CreateCStr, test_CreateCStr_len_zero)
{
    uint32 len = 0;
    char* str = CreateCStr(len);
    ASSERT_TRUE((str == NULL));

    delete [] str;
}

TEST(CreateCStr, test_CreateCStr_len_larger_zero)
{
    uint32 len = 10;
    char* str = CreateCStr(len);
    for (int i = 0; i < len; i++)
    {
        ASSERT_EQ(str[i], 0);
    }

    delete [] str;
}

const string WHITE_SPACES = " \r\n";
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

