#include <gtest/gtest.h>
#include <iostream>

using namespace std;

class IKTest : public testing::Test {
protected:
    // You can remove any or all of the following functions if its body
    // is empty.

    /***********************************************************/
    /***************** IK Test Unit Test Set-Up ****************/
    /***********************************************************/

    IKTest() {
        // To run before all tests.
    }

    void SetUp() {
        // To run before each test.
    }

    /***********************************************************/
    /*************** IK Test Unit Test Tear-Down ***************/
    /***********************************************************/

    void TearDown() {
        // To run after each test.
    }

    ~IKTest() {
        // To run after all tests.
    }
};

/***********************************************************/
/************** Test Unit Test Infrastructure **************/
/***********************************************************/

// Basic IK unit test. To be removed when other unit tests are added.
TEST_F(IKTest, BasicUnitTest) { ASSERT_TRUE(true); }