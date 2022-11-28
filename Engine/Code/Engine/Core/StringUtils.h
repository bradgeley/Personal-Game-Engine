// Bradley Christensen - 2022
#pragma once
#include <string>


//----------------------------------------------------------------------------------------------------------------------
// String Utils
//
// Common functions for using strings
//

template<typename ...Args>
std::string StringF(char const* format, Args ... args)
{
    char buffer[1024];
    snprintf(buffer, sizeof(buffer), format, args...);
    return buffer;
}

