// Bradley Christensen - 2022-2023
#include "StringUtils.h"

#include "ErrorUtils.h"
#include "Engine/Debug/DevConsole.h"
#include "Engine/Math/IntVec2.h"
#include "Engine/Math/MathUtils.h"



//----------------------------------------------------------------------------------------------------------------------
Strings SplitStringOnAnyDelimeter(std::string const& string, std::string const& delimiters)
{
    Strings result;

    std::string line = "";
    line.reserve(32);

    for (auto& c : string)
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
Strings SplitStringOnDelimeter(std::string const& string, char delimeter)
{
    Strings result;

    std::string line = "";
    line.reserve(32);
    
    for (auto& c : string)
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
std::string GetToLower(std::string const& string)
{
    std::string result;
    for (auto c : string)
    {
        if (c >= 'A' && c <= 'Z')
        {
            c -= ('A' - 'a');
        }
        result += c;
    }
    return result;
}



//----------------------------------------------------------------------------------------------------------------------
void ToLower(std::string& out_string)
{
    for (auto& c : out_string)
    {
        if (c >= 'A' && c <= 'Z')
        {
            c -= ('A' - 'a');
        }
    }
}



//----------------------------------------------------------------------------------------------------------------------
void ToUpper(std::string& out_string)
{
    for (auto& c : out_string)
    {
        if (c >= 'a' && c <= 'z')
        {
            c += ('A' - 'a');
        }
    }
}



//----------------------------------------------------------------------------------------------------------------------
// Move characters to the leftmost position possible until there is no whitespace in the string
// - Probably much more efficient than copying the string, filling it with no whitespace chars, then copying back.
void TrimWhitespace(std::string& out_string)
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
void TrimEdgeWhitespace(std::string& out_string)
{
    TrimLeadingWhitespace(out_string);
    TrimTrailingWhitespace(out_string);
}



//----------------------------------------------------------------------------------------------------------------------
void TrimLeadingWhitespace(std::string& out_string)
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
void TrimTrailingWhitespace(std::string& out_string)
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
bool IsWhitespace(char c)
{
    return (c == ' ') || (c == '\r') || (c == '\t') || (c == '\n') || (c == '\f');
}



//----------------------------------------------------------------------------------------------------------------------
bool DoesStringContain(std::string const& string, char c)
{
    for (auto& character : string)
    {
        if (character == c)
        {
            return true;
        }
    }
    return false;
}



//----------------------------------------------------------------------------------------------------------------------
bool StringToBool(std::string const& boolAsString)
{
    std::string lower = GetToLower(boolAsString);
    if (lower == "true" || lower == "1")
    {
        return true;
    }
    if (lower == "false" || lower == "0")
    {
        return false;
    }

    if (g_devConsole)
    {
        g_devConsole->LogWarningF("StringToBool: Arg {%s} did not contain a valid bool string (true, false, 1, 0)", boolAsString.c_str());
    }
    return false;
}



//----------------------------------------------------------------------------------------------------------------------
int StringToInt(std::string const& intAsString)
{
    return atoi(intAsString.c_str());
}



//----------------------------------------------------------------------------------------------------------------------
float StringToFloat(std::string const& floatAsString)
{
    return (float) atof(floatAsString.c_str());
}



//----------------------------------------------------------------------------------------------------------------------
Rgba8 StringToRgba8(std::string const& rgba8AsString)
{
    Rgba8 result;
    Strings asStrings = SplitStringOnDelimeter(rgba8AsString, ',');
    if (asStrings.size() <= 2 || asStrings.size() >= 5)
    {
        g_devConsole->LogErrorF("StringToRgba8: Arg {%s} did not contain 3-4 numbers 0-255 separated by commas e.g. (255,255,255,50)", rgba8AsString.c_str());
    }

    if (asStrings.size() >= 3)
    {
        result.r = (uint8_t) ClampInt(StringToInt(asStrings[0]), 0, 255);
        result.g = (uint8_t) ClampInt(StringToInt(asStrings[1]), 0, 255);
        result.b = (uint8_t) ClampInt(StringToInt(asStrings[2]), 0, 255);
    }
    if (asStrings.size() >= 4)
    {
        result.a = (uint8_t) ClampInt(StringToInt(asStrings[3]), 0, 255);
    }
    return result;
}



//----------------------------------------------------------------------------------------------------------------------
Vec2 StringToVec2(std::string const& vec2AsString)
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
IntVec2 StringToIntVec2(std::string const& intVec2AsString)
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
Strings StringToStrings(std::string const& stringsAsString)
{
    return SplitStringOnDelimeter(stringsAsString, ',');
}



//----------------------------------------------------------------------------------------------------------------------
bool DoesStringContainChar(std::string const& string, uint8_t character)
{
    for (auto& c : string)
    {
        if ((uint8_t) c == character)
        {
            return true;
        } 
    }
    return false;
}
