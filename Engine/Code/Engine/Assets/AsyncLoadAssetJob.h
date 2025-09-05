// Bradley Christensen - 2022-2025
#pragma once
#include "AssetLoaderFunction.h"
#include "AssetID.h"
#include "Engine/Core/Name.h"
#include "Engine/Multithreading/Job.h"



//----------------------------------------------------------------------------------------------------------------------
class AsyncLoadAssetJob : public Job
{
public:

    void Execute() override;
    void Complete() override;

public:

    Name m_assetName                    = Name::s_invalidName;
    AssetID m_assetID                   = INVALID_ASSET_ID;
    IAsset* m_loadedAsset               = nullptr;
    AssetLoaderFunction m_loaderFunc    = nullptr;
};