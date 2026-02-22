// Bradley Christensen - 2022-2026
#include "Engine/Renderer/Texture.h"
#include "Engine/Renderer/Renderer.h"
#include "Engine/Assets/AssetManager.h"
#include "Engine/Assets/Image.h"



//----------------------------------------------------------------------------------------------------------------------
bool Texture::CreateUniformTexture(IntVec2 const& dims, Rgba8 const& tint)
{
    Image image(dims, tint);
    return CreateFromImage(image, false);
}



//----------------------------------------------------------------------------------------------------------------------
IntVec2 Texture::GetDimensions() const
{
    return m_dimensions;
}