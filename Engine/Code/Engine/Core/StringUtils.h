// Bradley Christensen - 2022-2023
#pragma once
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
// Splits a string upon reaching any of the characters inside the delimeters string
//
//Strings SplitStringOnDelimeter(std::string const& string, std::string const& delimeters);
bool DoesStringContainChar(std::string const& string, uint8_t character);



//----------------------------------------------------------------------------------------------------------------------
template<typename ...Args>
std::string StringF(char const* format, Args ... args)
{
    char buffer[1024];
    snprintf(buffer, sizeof(buffer), format, args...);
    return buffer;
}