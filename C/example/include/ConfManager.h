#ifndef CONFMANAGER_H_
#define CONFMANAGER_H_

#include "Common.h"
#include <cstdlib>
#include <pthread.h>

const std::string SERVER_NAME = "FastServer/1.0.0.0";
const std::string SERVER_PROTOCOL = "HTTP/1.1";
const int32 DEFAULT_PORT = 80;
const int32 DEFAULT_WORK_THREAD_MAX_CNT = 10;
const int32 DEFAULT_MAX_CONN_CNT = 1024;
const std::string DEFAULT_ROOT_PATH = "./";
const uint32 DEFAULT_MAX_FILE_SIZE = 10485760; // 10MB

class ConfManager
{
    private:

        std::map<std::string, std::string> _params;

        ConfManager();

        static pthread_mutex_t _mutex;

        static ConfManager* _instance;

    public:

        static ConfManager* Instance();

        void Init();

        std::string GetServerName()
        {
            return SERVER_NAME;
        }

        std::string GetServerProtocol()
        {
             return SERVER_PROTOCOL;
        }

        uint32 GetPort()
        {
            return (_params.find("PORT") == _params.end()
                    || atoi(_params["PORT"].c_str()) == 0)
                ? DEFAULT_PORT
                : atoi(_params["PORT"].c_str());
        }

        uint32 GetWorkThreadCnt()
        {
            return (_params.find("WORK_THREAD_COUNT") == _params.end()
                    || atoi(_params["WORK_THREAD_COUNT"].c_str()) == 0)
                ? DEFAULT_WORK_THREAD_MAX_CNT
                : atoi(_params["WORK_THREAD_COUNT"].c_str());
        }

        uint32 GetMaxConnCnt()
        {
            return (_params.find("MAX_CONNECTION_COUNT") == _params.end()
                    || atoi(_params["MAX_CONNECTION_COUNT"].c_str()) == 0)
                ? DEFAULT_MAX_CONN_CNT
                : atoi(_params["MAX_CONNECTION_COUNT"].c_str());
        }
};


#endif
