#ifndef HTTPREQUEST_H_
#define HTTPREQUEST_H_

#include "Common.h"
#include <map>
#include <string>

//TODO: Will add GET parameters
class HttpRequest
{
    private:

        std::string _clientIP;

        uint16 _clientPort;

        /**
         * @brief: HttpMethod
         * e.g
         *      GET / POST / HEAD / PUT / ...
         **/
        HTTP_METHOD _method;

        /**
         * @brief: URL
         * e.g
         *      /index.html?k1=v1&k2=v2
         *      _url: /index.html
         **/
        std::string _url;

        /**
         * @brief: File extion name
         * e.g
         *      /index.html?k1=v1&k2=v2
         *      _fileExt: html
         **/
         std::string _fileExt;

        /**
         * @brief: URL
         * e.g
         *      /index.html?k1=v1&k2=v2
         *      _url: /index.html?k1=v1&k2=v2l
         **/
        std::string _urlAll;

        /**
         * @brief: Http version
         * e.g
         *      HTTP/1.0
         **/
        std::string _version;

        /**
         * @brief: Store GET string
         * e.g
         *      /index.html?k1=v1&k2=v2
         *      _getStr: k1=v1&k2=v2
        **/
        std::string _getStr;

        /**
         * @brief: Store GET parameters
         * e.g
         *      /index.html?k1=v1&k2=v2
         *      _get: (<k1, v1>, <k2, v2>)
         **/
        std::map<std::string, std::string> _get;

        /**
         * @brief: Store POST parameters
         * e.g
         *      k1=v1&k2=v2
         *      _post: (<k1, v1>, <k2, v2>)
         **/
        std::map<std::string, std::string> _post;

        /**
         * @brief: head parameters
         * e.g
         *      Content-Length: 1024
         *      Accept: text/html
         **/
        std::map<std::string, std::string> _headParams;

        /**
         * @brief: post message body
         **/
        char* _postData;

        /**
         * @brief: post message body length
         **/
        uint32 _postLen;

    public:
        HttpRequest();

        ~HttpRequest();

        HTTP_METHOD GetMethod() const { return _method; }

        std::string GetClientIP() const { return _clientIP; }

        uint16 GetClientPort() const { return _clientPort; }

        /**
         * @brief: Get url in request
         *  case-sensitive : true
         **/
        std::string GetUrl() const { return _url; }

        std::string GetFileExt() const { return _fileExt; }

        std::string GetUrlAll() const { return _urlAll; }

        std::string GetGetStr() const { return _getStr; }

        std::string GetVersion() const { return _version; }

        std::map<std::string, std::string> GetHeadParams() const { return _headParams; }

        char* GetPostData() const { return _postData; }

        uint32 GetPostDataLen() const { return _postLen; }

        void SetClientIP(std::string clientIP) { _clientIP = clientIP; }

        void SetClientPort(uint16 clientPort) { _clientPort = clientPort; }

        void SetUrl(std::string url) { _url = url; }

        void SetGET(std::map<std::string, std::string>& params)
        {
            _get = params;
        }

        void SetPOST(std::map<std::string, std::string>& params)
        {
            _post = params;
        }

        void SetPostDataLen(uint32 postLen) { _postLen = postLen; }

        void SetPostData(char* postData) { _postData = postData; }

        std::string ToString() const;

        static HttpRequest* CreateHttpRequest(const char* data, uint32 len);

        /**
         * @brief: Cut Http Head line from head, and return data left
         * @param [in] data: data
         * @param [out] request: HttpRequest which will be set method / url / version
         * @return: params + post_data
         * e.g
         *  GET / HTTP/1.0\r\n
            User-Agent: Mozilla/4.0 (compatible; MSIE 6.0;\r\n
            Host: www.baidu.com\r\n
            \r\n
            POST_DATA
            =>
            User-Agent: Mozilla/4.0 (compatible; MSIE 6.0;\r\n
            Host: www.baidu.com\r\n
            \r\n
            POST_DATA
         **/
        static const char* CutHttpHead(const char* data, HttpRequest* request);

        /**
         * @brief: Cut Http param lines from head, and return data left
         * @param [in] data: data
         * @param [out] request: HttpRequest which will be set params
         * @return: post data
         * e.g
            User-Agent: Mozilla/4.0 (compatible; MSIE 6.0;\r\n
            Host: www.baidu.com\r\n
            \r\n
            POST_DATA
            =>
            POST_DATA
         **/
        static const char* CutHttpParams(const char* data, HttpRequest* request);
};

#endif
