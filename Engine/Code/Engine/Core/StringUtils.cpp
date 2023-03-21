// Bradley Christensen - 2022-2023
#include "StringUtils.h"

#include <algorithm>



//----------------------------------------------------------------------------------------------------------------------
//Strings SplitStringOnDelimeter(std::string const& string, std::string const& delimeters)
//{
//    Strings result;
//
//    std::string line = "";
//    //for (auto& c : string)
//    {
//        //if ()
//    }
//    
//    return result;
//}



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
bool IsWhitespace(char c)
{
    return c != ' ' && c != '\r' && c != '\t';
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
