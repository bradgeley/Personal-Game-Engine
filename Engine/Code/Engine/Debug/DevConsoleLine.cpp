// Bradley Christensen - 2023
#include "DevConsoleLine.h"



//----------------------------------------------------------------------------------------------------------------------
DevConsoleLine::DevConsoleLine(std::string const& line, Rgba8 const& tint) : std::string(line), m_tint(tint)
{
}
