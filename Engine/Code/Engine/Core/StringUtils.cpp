// Bradley Christensen - 2022-2025
#include "StringUtils.h"
#include "ErrorUtils.h"
#include "Engine/Debug/DevConsole.h"
#include "Engine/Math/IntVec2.h"
#include "Engine/Math/MathUtils.h"



//----------------------------------------------------------------------------------------------------------------------
constexpr int ASCII_UPPER_TO_LOWER = 'a' - 'A'; // Dest (lower) - Source (upper) = upper -> lower
constexpr int ASCII_LOWER_TO_UPPER = 'A' - 'a'; // Dest (upper) - Source (lower) = lower -> upper




//----------------------------------------------------------------------------------------------------------------------
Strings StringUtils::SplitStringOnAnyDelimeter(std::string const& string, std::string const& delimiters)
{
    Strings result;

    std::string line = "";
    line.reserve(string.size());

    for (char const& c : string)
    {
        if (DoesStringContain(delimiters, c))
        {
            result.emplace_back(line);
            line.clear();
            continue;
        }
        line += c;
    }

    result.emplace_back(line);

    return result;
}



//----------------------------------------------------------------------------------------------------------------------
Strings StringUtils::SplitStringOnDelimeter(std::string const& string, char delimeter)
{
    Strings result;

    std::string line = "";
    line.reserve(string.size());
    
    for (char const& c : string)
    {
        if (c == delimeter)
        {
            result.emplace_back(line);
            line.clear();
            continue;
        }
        line += c;
    }

    result.emplace_back(line);
    
    return result;
}



//----------------------------------------------------------------------------------------------------------------------
std::string StringUtils::GetToLower(std::string const& string)
{
    std::string result;
    result.reserve(string.size());
    for (char c : string)
    {
        if (IsUpper(c))
        {
            c += ASCII_UPPER_TO_LOWER;
        }
        result += c;
    }
    return result;
}



//----------------------------------------------------------------------------------------------------------------------
void StringUtils::ToLower(std::string& out_string)
{
    for (char& c : out_string)
    {
        if (IsUpper(c))
        {
            c += ASCII_UPPER_TO_LOWER;
        }
    }
}



//----------------------------------------------------------------------------------------------------------------------
char StringUtils::ToLowerChar(char c)
{
    if (IsUpper(c))
    {
        return c + ASCII_UPPER_TO_LOWER;
    }
    return c;
}



//----------------------------------------------------------------------------------------------------------------------
void StringUtils::ToUpper(std::string& out_string)
{
    for (char& c : out_string)
    {
        if (IsLower(c))
        {
            c += ASCII_LOWER_TO_UPPER;
        }
    }
}



//----------------------------------------------------------------------------------------------------------------------
char StringUtils::ToUpperChar(char c)
{
    if (IsLower(c))
    {
        return c + ASCII_LOWER_TO_UPPER;
    }
    return c;
}



//----------------------------------------------------------------------------------------------------------------------
// Move characters to the leftmost position possible until there is no whitespace in the string
// - Probably much more efficient than copying the string, filling it with no whitespace chars, then copying back.
void StringUtils::TrimWhitespace(std::string& out_string)
{
    int noWhitespaceIndex = 0;
    for (int i = 0; i < (int) out_string.size(); ++i)
    {
        char& c = out_string[i];
        if (!IsWhitespace(c))
        {
            out_string[noWhitespaceIndex] = c;
            noWhitespaceIndex++;
        }
    }
    out_string.resize(noWhitespaceIndex);
}



//----------------------------------------------------------------------------------------------------------------------
void StringUtils::TrimEdgeWhitespace(std::string& out_string)
{
    TrimLeadingWhitespace(out_string);
    TrimTrailingWhitespace(out_string);
}



//----------------------------------------------------------------------------------------------------------------------
void StringUtils::TrimLeadingWhitespace(std::string& out_string)
{
    int numLeadingWhitespace = 0;
    for (int i = 0; i < (int) out_string.size(); ++i)
    {
        char& c = out_string[i];
        if (!IsWhitespace(c))
        {
            numLeadingWhitespace = i;
            break;
        }
    }
    out_string.erase(out_string.begin(), out_string.begin() + numLeadingWhitespace);
}



//----------------------------------------------------------------------------------------------------------------------
void StringUtils::TrimTrailingWhitespace(std::string& out_string)
{
    int numTrailingWhitespace = 0;
    for (int i = (int) out_string.size() - 1; i >= 0; --i)
    {
        char& c = out_string[i];
        if (IsWhitespace(c))
        {
            numTrailingWhitespace++;
        }
        else break;
    }
    out_string.erase(out_string.end() - numTrailingWhitespace, out_string.end());
}



//----------------------------------------------------------------------------------------------------------------------
bool StringUtils::IsWhitespace(char c)
{
    return (c == ' ') || (c == '\r') || (c == '\t') || (c == '\n') || (c == '\f');
}



//----------------------------------------------------------------------------------------------------------------------
bool StringUtils::IsUpper(char c)
{
    return c >= 'A' && c <= 'Z';
}



//----------------------------------------------------------------------------------------------------------------------
bool StringUtils::IsLower(char c)
{
    return c >= 'a' && c <= 'z';
}



//----------------------------------------------------------------------------------------------------------------------
bool StringUtils::DoesStringContain(std::string const& string, char c)
{
    for (char const& character : string)
    {
        if (character == c)
        {
            return true;
        }
    }
    return false;
}



//----------------------------------------------------------------------------------------------------------------------
bool StringUtils::StringToBool(std::string const& boolAsString)
{
    std::string lower = GetToLower(boolAsString);
    if (lower == "true" || lower == "1" || lower == "yes")
    {
        return true;
    }
    if (lower == "false" || lower == "0" || lower == "no")
    {
        return false;
    }

    if (g_devConsole)
    {
        g_devConsole->LogWarningF("StringToBool: Arg {%s} did not contain a valid bool string (true/false, 1/0, yes/no)", boolAsString.c_str());
    }
    return false;
}



//----------------------------------------------------------------------------------------------------------------------
int StringUtils::StringToInt(std::string const& intAsString)
{
    return atoi(intAsString.c_str());
}



//----------------------------------------------------------------------------------------------------------------------
float StringUtils::StringToFloat(std::string const& floatAsString)
{
    return (float) atof(floatAsString.c_str());
}



//----------------------------------------------------------------------------------------------------------------------
Rgba8 StringUtils::StringToRgba8(std::string const& rgba8AsString)
{
    Rgba8 result;
    Strings asStrings = SplitStringOnDelimeter(rgba8AsString, ',');
    if (asStrings.size() <= 2 || asStrings.size() >= 5)
    {
        g_devConsole->LogErrorF("StringToRgba8: Arg {%s} did not contain 3-4 numbers 0-255 separated by commas e.g. (255,255,255,50)", rgba8AsString.c_str());
    }

    if (asStrings.size() >= 3)
    {
        result.r = (uint8_t) MathUtils::ClampInt(StringToInt(asStrings[0]), 0, 255);
        result.g = (uint8_t) MathUtils::ClampInt(StringToInt(asStrings[1]), 0, 255);
        result.b = (uint8_t) MathUtils::ClampInt(StringToInt(asStrings[2]), 0, 255);
    }
    if (asStrings.size() >= 4)
    {
        result.a = (uint8_t) MathUtils::ClampInt(StringToInt(asStrings[3]), 0, 255);
    }
    return result;
}



//----------------------------------------------------------------------------------------------------------------------
Vec2 StringUtils::StringToVec2(std::string const& vec2AsString)
{
    Vec2 result;
    Strings asStrings = SplitStringOnDelimeter(vec2AsString, ',');
    if (asStrings.size() <= 1 || asStrings.size() >= 3)
    {
        g_devConsole->LogErrorF("StringToVec2: Arg {%s} did not contain 2 floats separated by a commas e.g. {5.55,-6.876}", vec2AsString.c_str());
    }

    if (asStrings.size() >= 2)
    {
        result.x = StringToFloat(asStrings[0]);
        result.y = StringToFloat(asStrings[1]);
    }
    
    return result;
}



//----------------------------------------------------------------------------------------------------------------------
IntVec2 StringUtils::StringToIntVec2(std::string const& intVec2AsString)
{
    IntVec2 result;
    Strings asStrings = SplitStringOnDelimeter(intVec2AsString, ',');
    if (asStrings.size() <= 1 || asStrings.size() >= 3)
    {
        g_devConsole->LogErrorF("StringToIntVec2: Arg {%s} did not contain 2 ints separated by a commas e.g. {5,-6}", intVec2AsString.c_str());
    }

    if (asStrings.size() >= 2)
    {
        result.x = StringToInt(asStrings[0]);
        result.y = StringToInt(asStrings[1]);
    }
    
    return result;
}



//----------------------------------------------------------------------------------------------------------------------
Strings StringUtils::CSVToStrings(std::string const& stringsAsString)
{
    return SplitStringOnDelimeter(stringsAsString, ',');
}



//----------------------------------------------------------------------------------------------------------------------
bool StringUtils::DoesStringContainChar(std::string const& string, uint8_t character)
{
    for (char const& c : string)
    {
        if ((uint8_t) c == character)
        {
            return true;
        } 
    }
    return false;
}



//----------------------------------------------------------------------------------------------------------------------
std::size_t StringUtils::CaseInsensitiveStringHash::operator()(const std::string& input) const noexcept
{
    #if INTPTR_MAX == INT64_MAX
        constexpr size_t FNV_offset = 1469598103934665603ULL;
        constexpr size_t FNV_prime = 1099511628211ULL;
    #else
        constexpr size_t FNV_offset = 2166136261U;
        constexpr size_t FNV_prime = 16777619U;
    #endif

    size_t hash = FNV_offset;
    for (unsigned char c : input)
    {
        if (IsUpper(c))
        {
            c = c + ASCII_UPPER_TO_LOWER;
        }
        hash ^= c;
        hash *= FNV_prime;
    }
    return hash;
}



//----------------------------------------------------------------------------------------------------------------------
bool StringUtils::CaseInsensitiveStringEquals::operator()(const std::string& a, const std::string& b) const noexcept
{
    size_t aSize = a.size();
    size_t bSize = b.size();
    if (aSize != bSize)
    {
        return false;
    }

    for (size_t i = 0; i < aSize; ++i)
    {
        unsigned char ca = a[i];
        unsigned char cb = b[i];

        if (IsUpper(ca))
        {
            ca += ASCII_UPPER_TO_LOWER;
        }
        if (IsUpper(cb))
        {
            cb += ASCII_UPPER_TO_LOWER;
        }

        if (ca != cb)
        {
            return false;
        }
    }
    return true;
}
