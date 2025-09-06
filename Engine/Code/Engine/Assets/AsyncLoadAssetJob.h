// Bradley Christensen - 2022-2025
#pragma once
#include "AssetLoaderFunction.h"
#include "AssetID.h"
#include "AssetKey.h"
#include "Engine/Multithreading/Job.h"



class IAsset;



//----------------------------------------------------------------------------------------------------------------------
class AsyncLoadAssetJob : public Job
{
public:

    void Execute() override;
    bool Complete() override;

public:

    AssetKey m_assetKey;
    AssetID m_assetID                   = INVALID_ASSET_ID;
    IAsset* m_loadedAsset               = nullptr;
    AssetLoaderFunction m_loaderFunc    = nullptr;
};