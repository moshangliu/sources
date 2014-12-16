#include "HttpRequest.h"
#include <gtest/gtest.h>
#include <fcntl.h>

int main(int argc, char **argv)
{
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}

class Test_HttpRequest_Suite : public testing::Test
{
    protected:
        static void SetUpTestCase()
        {
            _data = "GET / HTTP/1.0\r\n"
                "User-Agent: Mozilla/4.0 (compatible; MSIE 6.0;\r\n"
                "Host: www.baidu.com\r\n"
                "\r\n"
                "POST_DATA_BEGIN\r\n\r\nPOST_DATA_END\r\n";

            _dataNoHead = "User-Agent: Mozilla/4.0 (compatible; MSIE 6.0;\r\n"
                "Host: www.baidu.com\r\n"
                "\r\n"
                "POST_DATA_BEGIN\r\n\r\nPOST_DATA_END\r\n";

            _postData = "POST_DATA_BEGIN\r\n\r\nPOST_DATA_END\r\n";

            _dataNoParams = "GET / HTTP/1.0\r\n"
                "\r\n"
                "POST_DATA_BEGIN\r\n\r\nPOST_DATA_END\r\n";
            _dataNoParamsHead = "\r\n"
                "POST_DATA_BEGIN\r\n\r\nPOST_DATA_END\r\n";
        }

        static void TearDownTestCase()
        {

        }

        static const char* _data;
        static const char* _dataNoHead;
        static const char* _postData;

        static const char* _dataNoParams;
        static const char* _dataNoParamsHead;
};

const char* Test_HttpRequest_Suite::_data = NULL;
const char* Test_HttpRequest_Suite::_dataNoHead = NULL;
const char* Test_HttpRequest_Suite::_postData = NULL;

const char* Test_HttpRequest_Suite::_dataNoParams = NULL;
const char* Test_HttpRequest_Suite::_dataNoParamsHead = NULL;

TEST_F(Test_HttpRequest_Suite, test_CutHttpHead)
{
    HttpRequest* request = new HttpRequest();
    const char* dataNoHead = HttpRequest::CutHttpHead(_data, request);

    ASSERT_STREQ(dataNoHead, _dataNoHead);
    ASSERT_EQ(request->GetMethod(), GET);
    ASSERT_EQ(request->GetUrl(), "/");
    ASSERT_EQ(request->GetVersion(), "HTTP/1.0");
}

TEST_F(Test_HttpRequest_Suite, test_CutHttpParams)
{
    HttpRequest* request = new HttpRequest();
    const char* postData = HttpRequest::CutHttpParams(_dataNoHead, request);

    ASSERT_STREQ(postData, _postData);
    ASSERT_EQ(request->GetHeadParams()["User-Agent"], "Mozilla/4.0 (compatible; MSIE 6.0;");
    ASSERT_EQ(request->GetHeadParams()["Host"], "www.baidu.com");
}

TEST_F(Test_HttpRequest_Suite, test_CreateHttpRequest)
{
    HttpRequest* request = new HttpRequest();
    const char* dataNoHead = HttpRequest::CutHttpHead(_data, request);
    const char* postData = HttpRequest::CutHttpParams(dataNoHead, request);

    ASSERT_STREQ(dataNoHead, _dataNoHead);
    ASSERT_EQ(request->GetMethod(), GET);
    ASSERT_EQ(request->GetUrl(), "/");
    ASSERT_EQ(request->GetVersion(), "HTTP/1.0");

    ASSERT_STREQ(postData, _postData);
    ASSERT_EQ(request->GetHeadParams()["User-Agent"], "Mozilla/4.0 (compatible; MSIE 6.0;");
    ASSERT_EQ(request->GetHeadParams()["Host"], "www.baidu.com");
}


/**
 * @brief: Test CreateHttpRequest using cstring with empty params
 * @e.g
 *      _dataNoParams = "GET / HTTP/1.0\r\n"
 *              "\r\n"
 *              "POST_DATA";
 **/
TEST_F(Test_HttpRequest_Suite, test_CreateHttpRequestNoParams)
{
    HttpRequest* request = new HttpRequest();
    const char* dataNoHead = HttpRequest::CutHttpHead(_dataNoParams, request);
    const char* postData = HttpRequest::CutHttpParams(dataNoHead, request);

    ASSERT_STREQ(dataNoHead, _dataNoParamsHead);
    ASSERT_STREQ(postData, _postData);
}

const int BUF_SIZE = 1024;
TEST_F(Test_HttpRequest_Suite, test_CrtHttpReqFromFd)
{
    char buf[BUF_SIZE];
    memset(buf, 0, BUF_SIZE);

    int fd = open("../data/HttpRequest", O_RDONLY);
    int n = readn(fd, buf, BUF_SIZE);
    close(fd);

    ASSERT_GT(n, 0);
    HttpRequest* request = HttpRequest::CreateHttpRequest(buf, n);
    ASSERT_TRUE(request != NULL);
    ASSERT_EQ(request->GetMethod(), GET);
    ASSERT_EQ(request->GetUrl(), "/");
    ASSERT_EQ(request->GetVersion(), "HTTP/1.0");
    ASSERT_EQ(request->GetHeadParams()["User-Agent"], "Mozilla/4.0 (compatible; MSIE 6.0;");
    ASSERT_EQ(request->GetHeadParams()["Host"], "www.baidu.com");
    ASSERT_STREQ(request->GetPostData(), _postData);

    delete request;
}
