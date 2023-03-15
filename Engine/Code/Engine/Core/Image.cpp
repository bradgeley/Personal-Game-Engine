// Bradley Christensen - 2022-2023
#include "Engine/Core/Image.h"
#include "ErrorUtils.h"
#include "StringUtils.h"
#define STB_IMAGE_IMPLEMENTATION
#include "ThirdParty/stb/stb_image.h"



//----------------------------------------------------------------------------------------------------------------------
Image::Image(IntVec2 const& dimensions, Rgba8 const& color)
    : Grid2D<Rgba8>(dimensions, color), m_sourceImagePath("None")
{
    
}



//----------------------------------------------------------------------------------------------------------------------
Image::Image(char const* imageFilePath)
{
    LoadFromFile(imageFilePath);   
}



//----------------------------------------------------------------------------------------------------------------------
bool Image::LoadFromFile(const char* imageFilePath, bool putOriginAtBottomLeft)
{
    ASSERT_OR_DIE(m_data.empty(), "Trying to load an image from disc when it already is initialized. Not supported.")
    m_sourceImagePath = imageFilePath;
    
    int numComponents = 0;
    int numComponentsRequested = 0;
    
    stbi_set_flip_vertically_on_load(putOriginAtBottomLeft);
    uint8_t* imageData = stbi_load(
        imageFilePath,
        &m_dimensions.x,
        &m_dimensions.y,
        &numComponents,
        numComponentsRequested);

    ASSERT_OR_DIE(imageData != nullptr, StringF("Could not read image at path %s", imageFilePath))
    ASSERT_OR_DIE(numComponents == 3 || numComponents == 4, "Unsupported image format")

    if (imageData)
    {
        size_t numPixels = static_cast<size_t>(m_dimensions.x) * static_cast<size_t>(m_dimensions.y);
        m_data.reserve(numPixels);

        uint8_t* pixelIterator = imageData;
        for (size_t i=0; i<numPixels; ++i)
        {
            uint8_t& r = pixelIterator[0];
            uint8_t& g = pixelIterator[1];
            uint8_t& b = pixelIterator[2];
            uint8_t a = (numComponents == 4) ? pixelIterator[3] : 255;
            m_data.emplace_back(r, g, b, a);
            pixelIterator += numComponents;
        }

        stbi_image_free(imageData);
        return true;
    }
    return false;
}



//----------------------------------------------------------------------------------------------------------------------
std::string const& Image::GetSourceImagePath() const
{
    return m_sourceImagePath;
}
