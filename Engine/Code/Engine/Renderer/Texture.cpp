// Bradley Christensen - 2022-2023
#include "Engine/Renderer/Texture.h"
#include "Engine/Core/Image.h"



//----------------------------------------------------------------------------------------------------------------------
bool Texture::CreateUniformTexture(IntVec2 const& dims, Rgba8 const& tint)
{
    Image image(dims, tint);
    return CreateFromImage(image, false);
}



//----------------------------------------------------------------------------------------------------------------------
bool Texture::LoadFromImageFile(const char* imageSource, bool createMipMap)
{
    Image image = Image();
    if (image.LoadFromFile(imageSource))
    {
        return CreateFromImage(image, createMipMap);
    }
    return false;
}



//----------------------------------------------------------------------------------------------------------------------
IntVec2 Texture::GetDimensions() const
{
    return m_dimensions;
}