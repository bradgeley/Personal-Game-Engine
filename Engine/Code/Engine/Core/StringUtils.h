// Bradley Christensen - 2022-2023
#pragma once
#include "Engine/Math/Vec2.h"
#include "Engine/Renderer/Rgba8.h"
#include <string>
#include <vector>



typedef std::vector<std::string> Strings;



//----------------------------------------------------------------------------------------------------------------------
// String Utils
//
// Common functions for using strings
//
std::string GetToLower(std::string const& string);
void ToLower(std::string& out_string);
void ToUpper(std::string& out_string);
void TrimWhitespace(std::string& out_string);
bool IsWhitespace(char c);



//----------------------------------------------------------------------------------------------------------------------
// Conversion
//
bool StringToBool(std::string const& boolAsString);
int StringToInt(std::string const& intAsString);
float StringToFloat(std::string const& floatAsString);
Rgba8 StringToRgba8(std::string const& rgba8AsString);
Vec2 StringToVec2(std::string const& vec2AsString);
IntVec2 StringToIntVec2(std::string const& intVec2AsString);
Strings StringToStrings(std::string const& stringsAsString);



//----------------------------------------------------------------------------------------------------------------------
// Splits a string upon reaching any of the characters inside the delimeters string
//
Strings SplitStringOnDelimeter(std::string const& string, char delimeter);
bool DoesStringContainChar(std::string const& string, uint8_t character);



//----------------------------------------------------------------------------------------------------------------------
template<typename ...Args>
std::string StringF(char const* format, Args ... args)
{
    char buffer[1024];
    snprintf(buffer, sizeof(buffer), format, args...);
    return buffer; 
}