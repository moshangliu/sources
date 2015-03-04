#ifndef  __PLGTIMEOUT_H_
#define  __PLGTIMEOUT_H_

#include <time.h>
#include <gtest/gtest.h>

//断言超过时间作为失败返回
#define EXPECT_TIMEOUT(func_name, timeout) \
    { \
        try { \
            clock_t start, finish; \
            long double time_elapsed, timeout_limit; \
            timeout_limit = (long double) timeout; \
            start = clock(); \
            (func_name); \
            finish = clock(); \
            time_elapsed = (long double)(finish - start) / CLOCKS_PER_SEC * 1000; \
            EXPECT_LE(time_elapsed, timeout_limit); \
        } \
        catch( ... ) {} \
    }

#endif  //__PLGTIMEOUT_H_
