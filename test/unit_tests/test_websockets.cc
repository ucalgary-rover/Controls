#include <gtest/gtest.h>
#include <iostream>

using namespace std;

class WebsocketsTest : public testing::Test {
protected:
    // You can remove any or all of the following functions if its body
    // is empty.

    /***********************************************************/
    /************* Websockets Test Unit Test Set-Up ************/
    /***********************************************************/

    WebsocketsTest() {
        // To run before all tests.
    }

    void SetUp() {
        // To run before each test.
    }

    /***********************************************************/
    /*********** Websockets Test Unit Test Tear-Down ***********/
    /***********************************************************/

    void TearDown() {
        // To run after each test.
    }

    ~WebsocketsTest() {
        // To run after all tests.
    }
};

/***********************************************************/
/************** Test Unit Test Infrastructure **************/
/***********************************************************/

// Basic Websockets unit test. To be removed when other unit tests are added.
TEST_F(WebsocketsTest, BasicUnitTest) { ASSERT_TRUE(true); }