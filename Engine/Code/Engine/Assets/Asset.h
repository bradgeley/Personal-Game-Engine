// Bradley Christensen - 2022-2025
#pragma once
#include "Engine/Core/Name.h"
#include "AssetID.h"



//----------------------------------------------------------------------------------------------------------------------
class IAsset
{
	friend class AssetManager; 
	friend class AsyncLoadAssetJob;

protected:

	virtual ~IAsset() = default;
	virtual void ReleaseResources() = 0;

protected:

	Name		m_name;
	AssetID		m_assetID = INVALID_ASSET_ID;
};