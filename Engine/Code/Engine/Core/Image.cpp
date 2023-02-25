// Bradley Christensen - 2022-2023
#include "Engine/Core/Image.h"



Image::Image(Rgba8 const& color, IntVec2 const& dimensions)
    : m_sourceImagePath("None"), m_dimensions(dimensions)
{
    m_pixels.resize((size_t) dimensions.x * dimensions.y);
    memset(m_pixels.data(), (int) color.GetAsUint32(), m_pixels.size() * sizeof(Rgba8));
}



Image::Image(char const* imageFilePath)
{
    
}



bool Image::LoadFromFile(const char* imageFilePath, bool flipOriginToBottomLeft)
{
    
}
