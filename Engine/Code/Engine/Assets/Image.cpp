// Bradley Christensen - 2022-2025
#include "Engine/Assets/Image.h"
#include "Engine/Core/ErrorUtils.h"
#include "Engine/Core/StringUtils.h"

#define STB_IMAGE_IMPLEMENTATION
#include "ThirdParty/stb/stb_image.h"



//----------------------------------------------------------------------------------------------------------------------
Image::Image(IntVec2 const& dimensions, Rgba8 const& color)
    : m_pixels(dimensions, color)
{
    
}



//----------------------------------------------------------------------------------------------------------------------
void Image::ReleaseResources()
{
    m_pixels.Clear();
}



//----------------------------------------------------------------------------------------------------------------------
Grid<Rgba8> const& Image::GetPixels() const
{
    return m_pixels;
}



//----------------------------------------------------------------------------------------------------------------------
Grid<Rgba8>& Image::GetPixelsRef()
{
    return m_pixels;
}



//----------------------------------------------------------------------------------------------------------------------
Asset* Image::Load(Name assetName)
{
    Image* result = new Image();

    ASSERT_OR_DIE(result->m_pixels.m_data.empty(), "Trying to load an image from disc when it already is initialized. Not supported.");

    int numComponents = 0;
    int numComponentsRequested = 0;

    int dimsX = 0;
    int dimxY = 0;
    stbi_set_flip_vertically_on_load(true);
    uint8_t* imageData = stbi_load(
        assetName.ToCStr(),
        &dimsX,
        &dimxY,
        &numComponents,
        numComponentsRequested);

    ASSERT_OR_DIE(imageData != nullptr, StringUtils::StringF("Could not read image at path %s", assetName.ToCStr()));
    ASSERT_OR_DIE(numComponents == 3 || numComponents == 4, "Unsupported image format");

    if (imageData)
    {
        int numPixels = dimsX * dimxY;
        result->m_pixels.m_dimensions = IntVec2(dimsX, dimxY);
        result->m_pixels.m_data.reserve((size_t) numPixels);

        uint8_t* pixelIterator = imageData;
        for (int i = 0; i < numPixels; ++i)
        {
            uint8_t& r = pixelIterator[0];
            uint8_t& g = pixelIterator[1];
            uint8_t& b = pixelIterator[2];
            uint8_t a = (numComponents == 4) ? pixelIterator[3] : 255;
            result->m_pixels.m_data.emplace_back(r, g, b, a);
            pixelIterator += numComponents;
        }

        stbi_image_free(imageData);
    }

    return result;
}



//----------------------------------------------------------------------------------------------------------------------
bool Image::CompleteAsyncLoad()
{
    // Empty, all work done in Load
    return true;
}



//----------------------------------------------------------------------------------------------------------------------
bool Image::CompleteSyncLoad()
{
    // Empty, all work done in Load
    return true;
}
