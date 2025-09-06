// Bradley Christensen - 2022-2025
#pragma once
#include "AssetLoaderFunction.h"
#include "AssetID.h"
#include "AssetKey.h"
#include "Engine/Multithreading/Job.h"



class Asset;



//----------------------------------------------------------------------------------------------------------------------
class AsyncLoadAssetJob : public Job
{
public:

    void Execute() override;
    bool Complete() override;

public:

    AssetKey m_assetKey;
    AssetID m_assetID                   = AssetID::Invalid;
    Asset* m_loadedAsset                = nullptr;
    AssetLoaderFunction m_loaderFunc    = nullptr;
};