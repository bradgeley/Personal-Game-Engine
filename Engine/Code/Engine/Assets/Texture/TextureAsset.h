// Bradley Christensen - 2022-2025
#pragma once
#include "Engine/Renderer/RendererUtils.h"
#include "Engine/Assets/Asset.h"



//----------------------------------------------------------------------------------------------------------------------
class TextureAsset : public IAsset
{
public:

	static IAsset* Load(Name assetName);

	AssetID GetImageAssetID() const;
	TextureID GetTextureID() const;

protected:

	virtual bool CompleteAsyncLoad() override;
	virtual bool CompleteSyncLoad() override;
	virtual void ReleaseResources() override;

protected:

	AssetID	m_imageAssetID	= INVALID_ASSET_ID;
	TextureID m_textureID	= RendererUtils::InvalidID;
};