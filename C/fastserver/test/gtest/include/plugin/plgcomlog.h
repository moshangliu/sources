#ifndef  __PLGCOMLOG_H_
#define  __PLGCOMLOG_H_

#include <gtest/gtest.h>

// EXPECT comlog日志中含有某字符串,支持通配
#define EXPECT_COMLOG_INCLUDE(filename, pattern) \
    { \
        try { \
            char* cmd = "result=$(cat "#filename" | grep "#pattern"); \
				   if [ -z \"$result\" ];then \
					  echo 0 > /tmp/btest_plgcomlog; \
				   else \
					  echo 1 > /tmp/btest_plgcomlog; \
				   fi"; \
            system(cmd); \
            FILE *fp; \
            char ch; \
            if((fp=fopen("/tmp/btest_plgcomlog", "r")) != NULL) { \
                ch=fgetc(fp); \
            }; \
            fclose(fp); \
            if ( ch == '0' ) { \
                EXPECT_TRUE(false); \
            } \
            else \
            { \
                EXPECT_TRUE(true); \
            } \
        } \
        catch( ... ) \
        { }\
    }

// EXPECT comlog日志中不含某字符串,支持通配
#define EXPECT_COMLOG_EXCLUDE(filename, pattern) \
    { \
        try { \
            char* cmd = "result=$(cat "#filename" | grep "#pattern"); \
				   if [ -z \"$result\" ];then \
					  echo 0 > /tmp/btest_plgcomlog; \
				   else \
					  echo 1 > /tmp/btest_plgcomlog; \
				   fi"; \
            system(cmd); \
            FILE *fp; \
            char ch; \
            if((fp=fopen("/tmp/btest_plgcomlog", "r")) != NULL) { \
                ch=fgetc(fp); \
            }; \
            fclose(fp); \
            if ( ch == '1' ) { \
                EXPECT_FALSE(true); \
            } \
            else \
            { \
                EXPECT_FALSE(false); \
            } \
        } \
        catch( ... ) \
        { }\
    }

// ASSERT comlog日志中含有某字符串,支持通配
#define ASSERT_COMLOG_INCLUDE(filename, pattern) \
    { \
        try { \
            char* cmd = "result=$(cat "#filename" | grep "#pattern"); \
				   if [ -z \"$result\" ];then \
					  echo 0 > /tmp/btest_plgcomlog; \
				   else \
					  echo 1 > /tmp/btest_plgcomlog; \
				   fi"; \
            system(cmd); \
            FILE *fp; \
            char ch; \
            if((fp=fopen("/tmp/btest_plgcomlog", "r")) != NULL) { \
                ch=fgetc(fp); \
            }; \
            fclose(fp); \
            if ( ch == '0' ) { \
                ASSERT_TRUE(false); \
            } \
            else \
            { \
                ASSERT_TRUE(true); \
            } \
        } \
        catch( ... ) \
        { } \
    }

// ASSERT comlog日志中不含某字符串,支持通配
#define ASSERT_COMLOG_EXCLUDE(filename, pattern) \
    { \
        try { \
            char* cmd = "result=$(cat "#filename" | grep "#pattern"); \
				   if [ -z \"$result\" ];then \
					  echo 0 > /tmp/btest_plgcomlog; \
				   else \
					  echo 1 > /tmp/btest_plgcomlog; \
				   fi"; \
            system(cmd); \
            FILE *fp; \
            char ch; \
            if((fp=fopen("/tmp/btest_plgcomlog", "r")) != NULL) { \
                ch=fgetc(fp); \
            }; \
            fclose(fp); \
            if ( ch == '1' ) { \
                ASSERT_FALSE(true); \
            } \
            else \
            { \
                ASSERT_FALSE(false); \
            } \
        } \
        catch( ... ) \
        { }\
    }

#endif //__PLGCOMLOG_H_
