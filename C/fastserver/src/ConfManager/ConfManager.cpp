#include "ConfManager.h"
#include <cstring>
#include <fcntl.h>
#include <sys/stat.h>
#include <iostream>

using namespace std;

pthread_mutex_t ConfManager::_mutex = PTHREAD_MUTEX_INITIALIZER;
ConfManager* ConfManager::_instance = NULL;
const int32 BUF_SIZE = 1024;
const string WHITE_SPACES = " \r\n";

ConfManager::ConfManager()
{
    Init();
}

ConfManager* ConfManager::Instance()
{
    if (_instance == NULL)
    {
        pthread_mutex_lock(&_mutex);
        if (_instance == NULL)
        {
            _instance = new ConfManager();
        }
        pthread_mutex_unlock(&_mutex);
    }

    return _instance;
}

// TODO: init ConfManager with conf file
void ConfManager::Init()
{
    const char* path = "../conf/server.conf";
    int fd = open(path, O_RDONLY);
    struct stat fileInfo;
    memset(&fileInfo, 0, sizeof(struct stat));

    if (fd < 0 || fstat(fd, &fileInfo) < 0)
    {
        cerr << "Failed to load ./server.conf, maybe no authority or the file not exist." << endl;
        exit(0);
    }

    uint32 nleft = fileInfo.st_size;
    char* buf = CreateCStr(BUF_SIZE);

    while (nleft > 0)
    {
        memset(buf, 0, BUF_SIZE);
        int nread = readLine(fd, buf, BUF_SIZE);
        if (nread <= 0 || nread >= BUF_SIZE)
        {
            break;
        }

        nleft -= nread;

        string str = trim(string(buf), WHITE_SPACES);
        if (str.size() <= 0 || str[0] == '#') // Empty line / comment
        {
            continue;
        }

        vector<string> parts;
        Split(str, "=", parts);
        if (parts.size() != 2)
        {
            continue;
        }

        string key = trim(parts[0], WHITE_SPACES);
        string value = trim(parts[1], WHITE_SPACES);
        if (key.size() > 0 && value.size() > 0)
        {
            _params[key] = value;
        }
    }

    delete [] buf;
}
