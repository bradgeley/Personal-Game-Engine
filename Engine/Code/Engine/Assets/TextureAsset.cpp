// Bradley Christensen - 2022-2026
#include "TextureAsset.h"
#include "Engine/Assets/Image.h"
#include "Engine/Assets/AssetManager.h"
#include "Engine/Core/ErrorUtils.h"
#include "Engine/Renderer/Renderer.h"
#include "Engine/Renderer/Texture.h"



//----------------------------------------------------------------------------------------------------------------------
Asset* TextureAsset::Load(Name assetName)
{
	// This is the async part of the texture load, which is basically just creating the texture asset, but not any gpu resources yet.
    TextureAsset* textureAsset = new TextureAsset();
	textureAsset->m_name = assetName;

    if (g_renderer)
    {
        textureAsset->m_textureID = g_renderer->MakeTexture();
    }

    return textureAsset;
}



//----------------------------------------------------------------------------------------------------------------------
AssetID TextureAsset::GetImageAssetID() const
{
	return m_assetID;
}



//----------------------------------------------------------------------------------------------------------------------
TextureID TextureAsset::GetTextureID() const
{
	return m_textureID;
}



//----------------------------------------------------------------------------------------------------------------------
bool TextureAsset::CompleteAsyncLoad()
{
    if (m_imageAssetID == AssetID::Invalid)
    {
        m_imageAssetID = g_assetManager->AsyncLoad<Image>(m_name);
	}

    Image* loadedImage = g_assetManager->Get<Image>(m_imageAssetID);
    if (loadedImage == nullptr)
    {
        // Try again later
        return false;
    }

    Texture* texture = g_renderer->GetTexture(m_textureID);

    ASSERT_OR_DIE(texture, "TextureAsset::CompleteLoad - Failed to get texture from renderer.");
    if (!texture)
    {
        return false;
    }

    // GPU texture creation must happen on main thread
	bool succeeded = texture->CreateFromImage(*loadedImage, true);
	ASSERT_OR_DIE(succeeded, "TextureAsset::CompleteLoad - Failed to create texture from image.");

    g_assetManager->Release(m_imageAssetID);
    m_imageAssetID = AssetID::Invalid;

    return succeeded;
}



//----------------------------------------------------------------------------------------------------------------------
bool TextureAsset::CompleteSyncLoad()
{
    m_imageAssetID = g_assetManager->LoadSynchronous<Image>(m_name);

    Image* loadedImage = g_assetManager->Get<Image>(m_imageAssetID);
    if (loadedImage == nullptr)
    {
        return false;
    }

    Texture* texture = g_renderer->GetTexture(m_textureID);

    ASSERT_OR_DIE(texture, "TextureAsset::CompleteLoad - Failed to get texture from renderer.");
    if (!texture)
    {
        return false;
    }

    // GPU texture creation must happen on main thread
    bool succeeded = texture->CreateFromImage(*loadedImage, true);
    ASSERT_OR_DIE(succeeded, "TextureAsset::CompleteLoad - Failed to create texture from image.");

    g_assetManager->Release(m_imageAssetID);
    m_imageAssetID = AssetID::Invalid;

    return succeeded;
}



//----------------------------------------------------------------------------------------------------------------------
void TextureAsset::ReleaseResources()
{
    g_assetManager->Release(m_imageAssetID);
    if (g_renderer)
    {
        g_renderer->ReleaseTexture(m_textureID);
    }
}
