// Bradley Christensen - 2022-2026
#pragma once
#include "Engine/Renderer/RendererUtils.h"
#include "Engine/Assets/Asset.h"



//----------------------------------------------------------------------------------------------------------------------
class ShaderAsset : public Asset
{
public:

	static Asset* Load(Name assetName);

	AssetID GetImageAssetID() const;
	ShaderID GetShaderID() const;

protected:

	virtual bool CompleteAsyncLoad() override;
	virtual bool CompleteSyncLoad() override;
	virtual void ReleaseResources() override;

protected:

	ShaderID m_shaderID	= RendererUtils::InvalidID;
};