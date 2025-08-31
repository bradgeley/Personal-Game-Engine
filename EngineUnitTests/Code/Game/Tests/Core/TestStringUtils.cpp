// Bradley Christensen 2022-2025
#include "pch.h"
#include "Engine/Core/StringUtils.h"
#include "Engine/Math/Vec2.h"
#include "Engine/Math/IntVec2.h"
#include "Engine/Renderer/Rgba8.h"
#include <gtest/gtest.h>
#include <string>
#include <vector>

namespace TestStringUtils
{

    // SplitStringOnAnyDelimeter
    TEST(StringUtils, SplitStringOnAnyDelimeter)
    {
        std::string s = "a,b;c|d";
        std::string delims = ",;|";
        Strings result = StringUtils::SplitStringOnAnyDelimeter(s, delims);
        ASSERT_EQ(result.size(), 4);
        EXPECT_EQ(result[0], "a");
        EXPECT_EQ(result[1], "b");
        EXPECT_EQ(result[2], "c");
        EXPECT_EQ(result[3], "d");
    }

    // SplitStringOnDelimeter
    TEST(StringUtils, SplitStringOnDelimeter)
    {
        std::string s = "a,b,c";
        Strings result = StringUtils::SplitStringOnDelimeter(s, ',');
        ASSERT_EQ(result.size(), 3);
        EXPECT_EQ(result[0], "a");
        EXPECT_EQ(result[1], "b");
        EXPECT_EQ(result[2], "c");
    }

    // GetToLower
    TEST(StringUtils, GetToLower)
    {
        EXPECT_EQ(StringUtils::GetToLower("AbC123!"), "abc123!");
        EXPECT_EQ(StringUtils::GetToLower(""), "");
    }

    // ToLower
    TEST(StringUtils, ToLower)
    {
        std::string s = "HeLLo";
        StringUtils::ToLower(s);
        EXPECT_EQ(s, "hello");
    }

    // ToLowerChar
    TEST(StringUtils, ToLowerChar)
    {
        EXPECT_EQ(StringUtils::ToLowerChar('A'), 'a');
        EXPECT_EQ(StringUtils::ToLowerChar('z'), 'z');
        EXPECT_EQ(StringUtils::ToLowerChar('1'), '1');
    }

    // ToUpper
    TEST(StringUtils, ToUpper)
    {
        std::string s = "HeLLo";
        StringUtils::ToUpper(s);
        EXPECT_EQ(s, "HELLO");
    }

    // ToUpperChar
    TEST(StringUtils, ToUpperChar)
    {
        EXPECT_EQ(StringUtils::ToUpperChar('a'), 'A');
        EXPECT_EQ(StringUtils::ToUpperChar('Z'), 'Z');
        EXPECT_EQ(StringUtils::ToUpperChar('1'), '1');
    }

    // TrimWhitespace
    TEST(StringUtils, TrimWhitespace)
    {
        std::string s = " a b c ";
        StringUtils::TrimWhitespace(s);
        EXPECT_EQ(s, "abc");
    }

    // TrimEdgeWhitespace
    TEST(StringUtils, TrimEdgeWhitespace)
    {
        std::string s = "  hello world  ";
        StringUtils::TrimEdgeWhitespace(s);
        EXPECT_EQ(s, "hello world");
    }

    // TrimLeadingWhitespace
    TEST(StringUtils, TrimLeadingWhitespace)
    {
        std::string s = "   abc";
        StringUtils::TrimLeadingWhitespace(s);
        EXPECT_EQ(s, "abc");
    }

    // TrimTrailingWhitespace
    TEST(StringUtils, TrimTrailingWhitespace)
    {
        std::string s = "abc   ";
        StringUtils::TrimTrailingWhitespace(s);
        EXPECT_EQ(s, "abc");
    }

    // IsWhitespace
    TEST(StringUtils, IsWhitespace)
    {
        EXPECT_TRUE(StringUtils::IsWhitespace(' '));
        EXPECT_TRUE(StringUtils::IsWhitespace('\n'));
        EXPECT_TRUE(StringUtils::IsWhitespace('\t'));
        EXPECT_FALSE(StringUtils::IsWhitespace('a'));
    }

    // IsUpper
    TEST(StringUtils, IsUpper)
    {
        EXPECT_TRUE(StringUtils::IsUpper('A'));
        EXPECT_TRUE(StringUtils::IsUpper('Z'));
        EXPECT_FALSE(StringUtils::IsUpper('a'));
        EXPECT_FALSE(StringUtils::IsUpper('1'));
    }

    // IsLower
    TEST(StringUtils, IsLower)
    {
        EXPECT_TRUE(StringUtils::IsLower('a'));
        EXPECT_TRUE(StringUtils::IsLower('z'));
        EXPECT_FALSE(StringUtils::IsLower('A'));
        EXPECT_FALSE(StringUtils::IsLower('1'));
    }

    // DoesStringContain
    TEST(StringUtils, DoesStringContain)
    {
        EXPECT_TRUE(StringUtils::DoesStringContain("hello", 'e'));
        EXPECT_FALSE(StringUtils::DoesStringContain("hello", 'x'));
    }

    // StringToBool
    TEST(StringUtils, StringToBool)
    {
        EXPECT_TRUE(StringUtils::StringToBool("true"));
        EXPECT_TRUE(StringUtils::StringToBool("1"));
        EXPECT_TRUE(StringUtils::StringToBool("yes"));
        EXPECT_FALSE(StringUtils::StringToBool("false"));
        EXPECT_FALSE(StringUtils::StringToBool("0"));
        EXPECT_FALSE(StringUtils::StringToBool("no"));
        EXPECT_FALSE(StringUtils::StringToBool("notabool"));
    }

    // StringToInt
    TEST(StringUtils, StringToInt)
    {
        EXPECT_EQ(StringUtils::StringToInt("123"), 123);
        EXPECT_EQ(StringUtils::StringToInt("-42"), -42);
        EXPECT_EQ(StringUtils::StringToInt("0"), 0);
    }

    // StringToFloat
    TEST(StringUtils, StringToFloat)
    {
        EXPECT_FLOAT_EQ(StringUtils::StringToFloat("3.14"), 3.14f);
        EXPECT_FLOAT_EQ(StringUtils::StringToFloat("-2.5"), -2.5f);
        EXPECT_FLOAT_EQ(StringUtils::StringToFloat("0"), 0.0f);
    }

    // StringToRgba8
    TEST(StringUtils, StringToRgba8)
    {
        Rgba8 c = StringUtils::StringToRgba8("255,128,64,32");
        EXPECT_EQ(c.r, 255);
        EXPECT_EQ(c.g, 128);
        EXPECT_EQ(c.b, 64);
        EXPECT_EQ(c.a, 32);

        Rgba8 c2 = StringUtils::StringToRgba8("1,2,3");
        EXPECT_EQ(c2.r, 1);
        EXPECT_EQ(c2.g, 2);
        EXPECT_EQ(c2.b, 3);
        EXPECT_EQ(c2.a, 255);
    }

    // StringToVec2
    TEST(StringUtils, StringToVec2)
    {
        Vec2 v = StringUtils::StringToVec2("1.5,2.5");
        EXPECT_FLOAT_EQ(v.x, 1.5f);
        EXPECT_FLOAT_EQ(v.y, 2.5f);
    }

    // StringToIntVec2
    TEST(StringUtils, StringToIntVec2)
    {
        IntVec2 v = StringUtils::StringToIntVec2("10,-20");
        EXPECT_EQ(v.x, 10);
        EXPECT_EQ(v.y, -20);
    }

    // CSVToStrings
    TEST(StringUtils, CSVToStrings)
    {
        Strings s = StringUtils::CSVToStrings("a,b,c");
        ASSERT_EQ(s.size(), 3);
        EXPECT_EQ(s[0], "a");
        EXPECT_EQ(s[1], "b");
        EXPECT_EQ(s[2], "c");
    }

    // DoesStringContainChar
    TEST(StringUtils, DoesStringContainChar)
    {
        EXPECT_TRUE(StringUtils::DoesStringContainChar("abc", (uint8_t)'a'));
        EXPECT_FALSE(StringUtils::DoesStringContainChar("abc", (uint8_t)'z'));
    }

    // CaseInsensitiveStringHash
    TEST(StringUtils, CaseInsensitiveStringHash)
    {
        StringUtils::CaseInsensitiveStringHash hash;
        EXPECT_EQ(hash("Hello"), hash("hello"));
        EXPECT_EQ(hash("WORLD"), hash("world"));
        EXPECT_NE(hash("abc"), hash("def"));
    }

    // CaseInsensitiveStringEquals
    TEST(StringUtils, CaseInsensitiveStringEquals)
    {
        StringUtils::CaseInsensitiveStringEquals eq;
        EXPECT_TRUE(eq("Hello", "hello"));
        EXPECT_TRUE(eq("WORLD", "world"));
        EXPECT_FALSE(eq("abc", "def"));
        EXPECT_FALSE(eq("abc", "abcd"));
    }

}