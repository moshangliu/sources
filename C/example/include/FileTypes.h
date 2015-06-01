#ifndef FILETYPES_H_
#define FILETYPES_H_

#include "Common.h"
#include <map>
#include <string>
#include <pthread.h>

class FileTypes
{
    private:
        /**
         * @brief:  key - file extension
         *          value - type
         * e.g:
         *          key: html
         *          value: text/html
         **/
        std::map<std::string, std::string> _extToTypes;

        FileTypes();

        static FileTypes* _instance;

        static pthread_mutex_t _mutex;

    public:
        static FileTypes* Instance();

        void Init();

        /**
         * @brief: Check if file extension exists
         * @param [in] fileExt: file extension
         * @return: fileExt exists / not exists
         **/
        bool Exists(std::string fileExt) const;

        /**
         * @brief: Get file type by file extension
         * @param [in] fileExt: file extension
         * @return: file type
         *      e.g
         *          text/html
         **/
        std::string GetType(std::string fileExt);
};

#endif
