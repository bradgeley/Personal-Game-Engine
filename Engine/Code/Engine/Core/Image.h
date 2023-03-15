// Bradley Christensen - 2022-2023
#pragma once
#include "Engine/Math/Grid2D.h"
#include "Engine/Renderer/Rgba8.h"
#include <string>



struct IntVec2;



//----------------------------------------------------------------------------------------------------------------------
// Image
//
// A grid of pixels
//
class Image : public Grid2D<Rgba8>
{
public:
    
    Image() = default;
    explicit Image(IntVec2 const& dimensions, Rgba8 const& color);
    explicit Image(char const* imageFilePath);
    
    bool LoadFromFile(const char* imageFilePath, bool putOriginAtBottomLeft = true);

    std::string const& GetSourceImagePath() const;

private:

    std::string	m_sourceImagePath;
};
