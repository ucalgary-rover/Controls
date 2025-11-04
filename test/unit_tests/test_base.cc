#include <gtest/gtest.h>
#include <iostream>

using namespace std;

class BaseTest : public testing::Test {
protected:
    // You can remove any or all of the following functions if its body
    // is empty.

    /***********************************************************/
    /**************** Base Test Unit Test Set-Up ***************/
    /***********************************************************/

    BaseTest() {
        // To run before all tests.
    }

    void SetUp() {
        // To run before each test.
    }

    /***********************************************************/
    /************** Base Test Unit Test Tear-Down **************/
    /***********************************************************/

    void TearDown() {
        // To run after each test.
    }

    ~BaseTest() {
        // To run after all tests.
    }
};

/***********************************************************/
/************** Test Unit Test Infrastructure **************/
/***********************************************************/

// Basic base unit test. To be removed when other unit tests are added.
TEST_F(BaseTest, BasicUnitTest) { ASSERT_TRUE(true); }