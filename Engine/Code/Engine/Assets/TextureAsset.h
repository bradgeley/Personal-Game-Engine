// Bradley Christensen - 2022-2026
#pragma once
#include "Engine/Renderer/RendererUtils.h"
#include "Engine/Assets/Asset.h"



//----------------------------------------------------------------------------------------------------------------------
class TextureAsset : public Asset
{
public:

	static Asset* Load(Name assetName);

	AssetID GetImageAssetID() const;
	TextureID GetTextureID() const;

protected:

	virtual bool CompleteAsyncLoad() override;
	virtual bool CompleteSyncLoad() override;
	virtual void ReleaseResources() override;

protected:

	AssetID	m_imageAssetID	= AssetID::Invalid;
	TextureID m_textureID	= RendererUtils::InvalidID;
};