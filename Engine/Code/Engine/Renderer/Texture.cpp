// Bradley Christensen - 2022-2025
#include "Engine/Renderer/Texture.h"
#include "Engine/Assets/AssetManager.h"
#include "Engine/Assets/Image/Image.h"



//----------------------------------------------------------------------------------------------------------------------
bool Texture::CreateUniformTexture(IntVec2 const& dims, Rgba8 const& tint)
{
    Image image(dims, tint);
    return CreateFromImage(image, false);
}



//----------------------------------------------------------------------------------------------------------------------
bool Texture::LoadFromImageFile(const char* imageSource, bool createMipMap)
{
    AssetID imageID = g_assetManager->LoadSynchronous<Image>(imageSource);
    Image* image = g_assetManager->Get<Image>(imageID);

    return CreateFromImage(*image, createMipMap);
}



//----------------------------------------------------------------------------------------------------------------------
IntVec2 Texture::GetDimensions() const
{
    return m_dimensions;
}