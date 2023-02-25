// Bradley Christensen - 2022-2023
#pragma once
#include <string>
#include <vector>
#include "Engine/Math/IntVec2.h"
#include "Engine/Renderer/Rgba8.h"



struct Rgba8;



//----------------------------------------------------------------------------------------------------------------------
// Image
//
// A grid of pixels
//
class Image
{
public:
    
    Image() = default;
    explicit Image(Rgba8 const& color, IntVec2 const& dimensions);
    explicit Image(char const* imageFilePath);

    bool LoadFromFile(const char* imageFilePath, bool flipOriginToBottomLeft);

private:

    std::string	        m_sourceImagePath;
	IntVec2	            m_dimensions = IntVec2::ZeroVector;
	std::vector<Rgba8>	m_pixels;
    
};
