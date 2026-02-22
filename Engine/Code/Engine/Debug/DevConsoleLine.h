// Bradley Christensen - 2022-2026
#pragma once
#include "Engine/Renderer/Rgba8.h"
#include <string>



//----------------------------------------------------------------------------------------------------------------------
struct DevConsoleLine
{
public:

    DevConsoleLine() = default;
    DevConsoleLine(std::string const& line, Rgba8 const& tint);

public:

    std::string m_line;
    Rgba8 m_tint;
};