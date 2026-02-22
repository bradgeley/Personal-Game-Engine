// Bradley Christensen - 2022-2026
#pragma once
#include "Engine/Math/Vec2.h"
#include "Engine/Renderer/Rgba8.h"
#include <string>
#include <vector>



//----------------------------------------------------------------------------------------------------------------------
typedef std::vector<std::string> Strings;



//----------------------------------------------------------------------------------------------------------------------
// String Utils
//
// Common functions for using strings
//
namespace StringUtils
{
    std::string GetToLower(std::string const& string);
    void ToLower(std::string& out_string);
    char ToLowerChar(char c);
    void ToUpper(std::string& out_string);
    char ToUpperChar(char c);
    void TrimWhitespace(std::string& out_string);
    void TrimEdgeWhitespace(std::string& out_string); // Only trims on left and right of string, not central whitespace
    void TrimLeadingWhitespace(std::string& out_string); // Only trims on left of string, not central whitespace
    void TrimTrailingWhitespace(std::string& out_string); // Only trims on right of string, not central whitespace
    bool IsWhitespace(char c);
    bool IsUpper(char c);
    bool IsLower(char c);
    bool DoesStringContain(std::string const& string, char c);



    //----------------------------------------------------------------------------------------------------------------------
    // Conversion
    //
    bool StringToBool(std::string const& boolAsString);
    int StringToInt(std::string const& intAsString);
    float StringToFloat(std::string const& floatAsString);
    Rgba8 StringToRgba8(std::string const& rgba8AsString);
    Vec2 StringToVec2(std::string const& vec2AsString);
    IntVec2 StringToIntVec2(std::string const& intVec2AsString);
    Strings CSVToStrings(std::string const& stringsAsString);



    //----------------------------------------------------------------------------------------------------------------------
    // Splits a string upon reaching any of the characters inside the delimeters string
    //
    Strings SplitStringOnAnyDelimiter(std::string const& string, std::string const& delimiters);
    Strings SplitStringOnDelimiter(std::string const& string, char delimeter);
    bool DoesStringContainChar(std::string const& string, uint8_t character);



    //----------------------------------------------------------------------------------------------------------------------
    template<typename ...Args>
    std::string StringF(char const* format, Args ... args)
    {
        char buffer[1024];
        snprintf(buffer, sizeof(buffer), format, args...);
        return buffer;
    }



    //----------------------------------------------------------------------------------------------------------------------
    struct CaseInsensitiveStringHash
    {
        std::size_t operator()(const std::string& input) const noexcept;
    };



    //----------------------------------------------------------------------------------------------------------------------
    struct CaseInsensitiveStringEquals
    {
        bool operator()(const std::string& a, const std::string& b) const noexcept;
    };
}