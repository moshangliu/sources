#include "Common.h"
#include "SafePriorityQueue.h"

#include <string>
#include <gtest/gtest.h>

using namespace std;

int main(int argc, char **argv)
{
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}

class Object {
public:
    long timeStampMs;
    string value;
};

class ObjectComp4MaxHeap {
public:
    bool operator() (const Object* v1, const Object* v2) const {
        return (v1->timeStampMs) > (v2->timeStampMs);
    }
};

TEST(SafePriorityQueue, test_SafePriorityQueue_max) {
    SafePriorityQueue<Object*, vector<Object*>, ObjectComp4MaxHeap> data;
    ASSERT_TRUE(data.empty());
    ASSERT_EQ(0, data.size());
}
