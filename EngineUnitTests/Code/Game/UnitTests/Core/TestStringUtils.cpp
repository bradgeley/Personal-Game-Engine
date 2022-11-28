// Bradley Christensen - 2022
#include "gtest/gtest.h"
#include "Engine/Core/StringUtils.h"



static char const* testString = "Hello, World!";
static char const* testStringFormattedString = "Test String: %s";
static char const* testStringFormattedStringResult = "Test String: Hello, World!";

static char const* testIntFormattedString = "Test Int: %i";
static char const* testIntFormattedStringResult = "Test Int: 69";

static char const* testFloatFormattedString = "Test Float: %.1f";
static char const* testFloatFormattedStringResult = "Test Float: 4.0";



TEST(StringUtils, StringF)
{
    std::string test1 = StringF(testString);
    EXPECT_STREQ(test1.c_str(), testString);
    
    std::string test2 = StringF(testStringFormattedString, testString);
    EXPECT_STREQ(test2.c_str(), testStringFormattedStringResult);

    int testInt = 69;
    std::string test3 = StringF(testIntFormattedString, testInt);
    EXPECT_STREQ(test3.c_str(), testIntFormattedStringResult);

    float testFloat = 4.f;
    std::string test4 = StringF(testFloatFormattedString, testFloat);
    EXPECT_STREQ(test4.c_str(), testFloatFormattedStringResult);
}