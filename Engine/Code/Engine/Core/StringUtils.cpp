// Bradley Christensen - 2022-2023
#include "StringUtils.h"

#include <algorithm>



//----------------------------------------------------------------------------------------------------------------------
Strings SplitStringOnDelimeter(std::string const& string, std::string const& delimeters)
{
    Strings result;

    std::string line = "";
    for (auto& c : string)
    {
        //if ()
    }
    
    return result;
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
