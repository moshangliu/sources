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

TEST(GetHttpMethod, test_GetHttpMethod_right)
{
    ASSERT_EQ(GetHttpMethod("GET"), GET);
    ASSERT_EQ(GetHttpMethod("POST"), POST);
    ASSERT_EQ(GetHttpMethod("HEAD"), HEAD);
    ASSERT_EQ(GetHttpMethod("PUT"), PUT);
    ASSERT_EQ(GetHttpMethod("DELETE"), DELETE);
    ASSERT_EQ(GetHttpMethod("TRACE"), TRACE);
    ASSERT_EQ(GetHttpMethod("CONNECT"), CONNECT);
    ASSERT_EQ(GetHttpMethod("OPTIONS"), OPTIONS);
}

TEST(GetHttpMethod, test_GetHttpMethod_other)
{
    ASSERT_EQ(GetHttpMethod("NO_METHOD"), OTHER);
}

TEST(HttpMethodToString, test_HttpMethodToString_right)
{
    ASSERT_EQ(HttpMethodToString(GET), "GET");
    ASSERT_EQ(HttpMethodToString(POST), "POST");
    ASSERT_EQ(HttpMethodToString(HEAD), "HEAD");
    ASSERT_EQ(HttpMethodToString(PUT), "PUT");
    ASSERT_EQ(HttpMethodToString(DELETE), "DELETE");
    ASSERT_EQ(HttpMethodToString(TRACE), "TRACE");
    ASSERT_EQ(HttpMethodToString(CONNECT), "CONNECT");
    ASSERT_EQ(HttpMethodToString(OPTIONS), "OPTIONS");
    ASSERT_EQ(HttpMethodToString(OTHER), "OTHER");
}

TEST(WriteResponse, test_WriteResponse)
{
    HttpResponse response;
    ASSERT_EQ(WriteResponse(-1, response), -1);

    int fd = open("httpResponse.txt", O_WRONLY | O_CREAT);
    response.SetHttpVersion("HTTP/1.0");
    response.SetStatusAndReason("200", "OK");

    string post("POST_BEGIN\r\nPOST_MID\r\n\r\nPOST_END");
    response.SetStatusAndReason("200", "OK");
    response.SetHeaderParam("Content-Length", IntToStr(post.size()));

    // [response] will be responsible for free [data]
    char* data = CreateCStr(post.size() + 1);
    sprintf(data, "%s", post.c_str());
    response.SetBodyAndLen(data, post.size());
    ASSERT_EQ(WriteResponse(fd, response), 0);
    close(fd);

    fd = open("httpResponse.txt", O_RDONLY);
    if (fd < 0)
    {
        ASSERT_TRUE(false);
    }
    char* buf = CreateCStr(BUF_SIZE);
    readn(fd, buf, BUF_SIZE);

    string expected = "HTTP/1.0 200 OK\r\nContent-Length: 32\r\n"
        "\r\n"
        "POST_BEGIN\r\nPOST_MID\r\n\r\nPOST_END";
    ASSERT_STREQ(buf, expected.c_str());
    close(fd);
    delete [] buf;
}

const string HTML = "html";
TEST(GetFileExtension, test_GetFileExtension)
{
    ASSERT_EQ(GetFileExtension("/home/work/index.html"), HTML);
    ASSERT_EQ(GetFileExtension("/home/work/index."), EMPTY);
    ASSERT_EQ(GetFileExtension("/home/work/index"), EMPTY);
}

TEST(IntToStr, test_IntToStr)
{
    ASSERT_EQ(IntToStr(0), "0");
    ASSERT_EQ(IntToStr(-1), "-1");
    ASSERT_EQ(IntToStr(1024), "1024");
    ASSERT_EQ(IntToStr(-1024), "-1024");
    ASSERT_EQ(IntToStr(+1024), "1024");
}

TEST(GetPureUrl, test_GetPureUrl)
{
    string url = "/data/index.html?k1=v1&k2=v2";
    ASSERT_EQ(GetPureUrl(url), "/data/index.html");

    url = "/data/index.html";
    ASSERT_EQ(GetPureUrl(url), "/data/index.html");
}

TEST(GetParams, test_GetParams)
{
    string url = "/data/index.html?k1=v1&k2=v2";
    map<string, string> params = GetParams(url);
    ASSERT_EQ(params.size(), 2);
    ASSERT_EQ(params["k1"], "v1");
    ASSERT_EQ(params["k2"], "v2");

    url = "/data/index.html";
    params = GetParams(url);
    ASSERT_EQ(params.size(), 0);

    url = "/data/index.html?";
    params = GetParams(url);
    ASSERT_EQ(params.size(), 0);

    url = "/data/index.html?key";
    params = GetParams(url);
    ASSERT_EQ(params.size(), 0);

    url = "/data/index.html?key=";
    params = GetParams(url);
    ASSERT_EQ(params.size(), 0);

    url = "/data/index.html?key=value1=value2";
    params = GetParams(url);
    ASSERT_EQ(params.size(), 0);

    url = "/data/index.html?key=value1";
    params = GetParams(url);
    ASSERT_EQ(params.size(), 1);
    ASSERT_EQ(params["key"], "value1");
}

TEST(SetAndWriteBack, test_SetAndWriteBack_no_msgbody)
{
    int fd = open("./setAndWriteBack.txt", O_WRONLY | O_CREAT);
    if (fd < 0)
    {
        ASSERT_TRUE(false);
        return;
    }

    HttpResponse response;
    response.SetHttpVersion("HTTP/1.0");
    SetAndWriteBack(fd, response, "400", "Bad Request");
    close(fd);

    fd = open("./setAndWriteBack.txt", O_RDONLY);
    char* buf = CreateCStr(BUF_SIZE);
    readn(fd, buf, BUF_SIZE);

    const char* expected = "HTTP/1.0 400 Bad Request\r\n"
        "Content-Length: 0\r\n"
        "Content-Type: text/html\r\n"
        "\r\n";
    ASSERT_STREQ(buf, expected);
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

TEST(GetQueryStr, test_GetQueryStr_right)
{
    ASSERT_EQ(GetQueryStr("/index.html?key=value&key1=value1"), "key=value&key1=value1");
    ASSERT_EQ(GetQueryStr("/index.html"), EMPTY);
}

TEST(URLDecode, test_URLDecode)
{
    ASSERT_TRUE(URLDecode("testtext") == "testtext");
    ASSERT_TRUE(URLDecode("correctxt?text=utf-8%E4%B8%AD%E6%96%87%E6%B5%8B%E8%AF%95&version=0&ie=UTF-8&callback=callbackfunc") == "correctxt?text=utf-8中文测试&version=0&ie=UTF-8&callback=callbackfunc");
}
