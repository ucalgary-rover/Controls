#include <gtest/gtest.h>
#include <iostream>

using namespace std;

class QueueTest : public testing::Test {
protected:
    // You can remove any or all of the following functions if its body
    // is empty.

    /***********************************************************/
    /*************** Queue Test Unit Test Set-Up ***************/
    /***********************************************************/

    QueueTest() {
        // To run before all tests.
    }

    void SetUp() {
        // To run before each test.
    }

    /***********************************************************/
    /************* Queue Test Unit Test Tear-Down **************/
    /***********************************************************/

    void TearDown() {
        // To run after each test.
    }

    ~QueueTest() {
        // To run after all tests.
    }
};

/***********************************************************/
/************** Test Unit Test Infrastructure **************/
/***********************************************************/

// Basic queue unit test. To be removed when other unit tests are added.
TEST_F(QueueTest, BasicUnitTest) { ASSERT_TRUE(true); }