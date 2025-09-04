// Bradley Christensen - 2022-2025
#pragma once
#include "Engine/Core/Name.h"



//----------------------------------------------------------------------------------------------------------------------
typedef uint32_t AssetID;
constexpr AssetID INVALID_ASSET_ID = static_cast<AssetID>(UINT32_MAX);



//----------------------------------------------------------------------------------------------------------------------
class IAsset
{
	friend class AssetManager;

protected:

	virtual ~IAsset() = default;
	virtual void ReleaseResources() = 0;

protected:

	Name m_name;
};