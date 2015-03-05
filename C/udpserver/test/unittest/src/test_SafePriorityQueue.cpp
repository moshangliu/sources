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
    Object(long timeStampMs, string value)
        : _timeStampMs(timeStampMs), _value(value) {}
private:
    long _timeStampMs;
    string _value;

public:
    long timeStampMs() const { return _timeStampMs; }
    string value() const { return _value; }
};

class ObjectComp4MaxHeap {
public:
    bool operator() (const Object* v1, const Object* v2) const {
        return (v1->timeStampMs()) < (v2->timeStampMs());
    }
};

class ObjectComp4MinHeap {
public:
    bool operator() (const Object* v1, const Object* v2) const {
        return (v1->timeStampMs()) > (v2->timeStampMs());
    }
};

TEST(SafePriorityQueue, test_SafePriorityQueue_max_heap) {
    SafePriorityQueue<Object*, vector<Object*>, ObjectComp4MaxHeap> data;
    ASSERT_TRUE(data.empty());
    ASSERT_EQ(0, data.size());

    Object* obj1 = new Object(1, "one");
    data.push(obj1);
    ASSERT_EQ(1, data.size());

    Object* obj2 = new Object(2, "two");
    data.push(obj2);
    ASSERT_EQ(2, data.size());

    Object* obj3 = new Object(3, "three");
    data.push(obj3);
    ASSERT_EQ(3, data.size());

    Object* obj = data.top();
    ASSERT_TRUE(obj != NULL);
    ASSERT_EQ(obj->timeStampMs(), obj3->timeStampMs());
    ASSERT_EQ(obj->value(), obj3->value());

    data.pop();
    ASSERT_EQ(2, data.size());

    obj = data.top();
    ASSERT_TRUE(obj != NULL);
    ASSERT_EQ(obj->timeStampMs(), obj2->timeStampMs());
    ASSERT_EQ(obj->value(), obj2->value());

    data.pop();
    ASSERT_EQ(1, data.size());

    obj = data.top();
    ASSERT_TRUE(obj != NULL);
    ASSERT_EQ(obj->timeStampMs(), obj1->timeStampMs());
    ASSERT_EQ(obj->value(), obj1->value());

    data.pop();
    ASSERT_EQ(0, data.size());
}

TEST(SafePriorityQueue, test_SafePriorityQueue_min_heap) {
    SafePriorityQueue<Object*, vector<Object*>, ObjectComp4MinHeap> data;
    ASSERT_TRUE(data.empty());
    ASSERT_EQ(0, data.size());

    Object* obj1 = new Object(1, "one");
    data.push(obj1);
    ASSERT_EQ(1, data.size());

    Object* obj2 = new Object(2, "two");
    data.push(obj2);
    ASSERT_EQ(2, data.size());

    Object* obj3 = new Object(3, "three");
    data.push(obj3);
    ASSERT_EQ(3, data.size());

    Object* obj = data.top();
    ASSERT_TRUE(obj != NULL);
    ASSERT_EQ(obj->timeStampMs(), obj1->timeStampMs());
    ASSERT_EQ(obj->value(), obj1->value());

    data.pop();
    ASSERT_EQ(2, data.size());

    obj = data.top();
    ASSERT_TRUE(obj != NULL);
    ASSERT_EQ(obj->timeStampMs(), obj2->timeStampMs());
    ASSERT_EQ(obj->value(), obj2->value());

    data.pop();
    ASSERT_EQ(1, data.size());

    obj = data.top();
    ASSERT_TRUE(obj != NULL);
    ASSERT_EQ(obj->timeStampMs(), obj3->timeStampMs());
    ASSERT_EQ(obj->value(), obj3->value());

    data.pop();
    ASSERT_EQ(0, data.size());
}
