// Bradley Christensen - 2022-2025
#pragma once
#include "Engine/Core/Name.h"
#include "AssetID.h"



//----------------------------------------------------------------------------------------------------------------------
class Asset
{
	friend class AssetManager; 
	friend class AsyncLoadAssetJob;

public:

	Name GetName() const;

protected:

	virtual ~Asset() = default;

	virtual bool CompleteAsyncLoad()	= 0;
	virtual bool CompleteSyncLoad()		= 0;
	virtual void ReleaseResources()		= 0;

protected:

	Name		m_name;
	AssetID		m_assetID = AssetID::Invalid;
};