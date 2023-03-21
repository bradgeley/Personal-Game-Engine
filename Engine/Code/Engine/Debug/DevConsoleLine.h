// Bradley Christensen - 2023
#pragma once
#include "Engine/Renderer/Rgba8.h"
#include <string>



//----------------------------------------------------------------------------------------------------------------------
struct DevConsoleLine
{
    DevConsoleLine() = default;
    DevConsoleLine(std::string const& line, Rgba8 const& tint);

    std::string m_line;
    Rgba8 m_tint;
};