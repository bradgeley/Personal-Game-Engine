// Bradley Christensen - 2022-2025
#include "AssetManager.h"
#include "Engine/Core/EngineCommon.h"
#include "Engine/Core/ErrorUtils.h"
#include "Engine/Assets/Sprites/GridSpriteSheet.h"
#include "Engine/Assets/Sprites/SpriteAnimation.h"
#include "Engine/Debug/DevConsole.h"



//----------------------------------------------------------------------------------------------------------------------
// THE Asset Manager
//
AssetManager* g_assetManager = nullptr;



//----------------------------------------------------------------------------------------------------------------------
AssetManager::AssetManager(AssetManagerConfig const& config) : m_config(config)
{

}



//----------------------------------------------------------------------------------------------------------------------
void AssetManager::BeginFrame()
{
    // Begin frame happens synchronously on the main thread, and we require that all loads be completed on the main thread,
    // because stuff like textures need to create GPU resources when the load is completed
    for (auto it = m_futureAssets.begin(); it != m_futureAssets.end();)
    {
        FutureAsset const& futureAsset = it->second;
        if (g_jobSystem->CompleteJob(futureAsset.m_jobID, false))
        {
            it = m_futureAssets.erase(it);
        }
        else
        {
            ++it;
        }
	}
}



//----------------------------------------------------------------------------------------------------------------------
void AssetManager::Release(AssetID assetID)
{
    auto futureIt = m_futureAssets.find(assetID);
    if (futureIt != m_futureAssets.end())
    {
        bool cancelled = g_jobSystem->TryCancelLoadingJob(futureIt->second.m_jobID);
        #if defined(DEBUG_ASSET_MANAGER)
            g_devConsole->LogF(Rgba8::DarkOceanBlue, "- Attempted to cancel asset load job (%s): assedID(%i)", cancelled ? "Succeeded" : "Failed", static_cast<int>(assetID));
        #endif // DEBUG_ASSET_MANAGER
        if (!cancelled)
        {
            g_jobSystem->CompleteJob(futureIt->second.m_jobID, true);
        }
        m_futureAssets.erase(futureIt);
    }

    auto it = m_loadedAssets.find(assetID);
    if (it == m_loadedAssets.end())
    {
        #if defined(DEBUG_ASSET_MANAGER)
            g_devConsole->LogF(Rgba8::DarkOceanBlue, "- Trying to release asset that is unloaded: assedID(%i)", static_cast<int>(assetID));
        #endif // DEBUG_ASSET_MANAGER
        return;
    }

    #if defined(DEBUG_ASSET_MANAGER)
        g_devConsole->LogF(Rgba8::DarkOceanBlue, "- Releasing asset: assedID(%i) refCount(%i->%i)", static_cast<int>(assetID), it->second.m_refCount, it->second.m_refCount - 1);
    #endif // DEBUG_ASSET_MANAGER

    it->second.m_refCount--;

    if (it->second.m_refCount <= 0)
    {
        UnloadAsset(assetID);
    }
}



//----------------------------------------------------------------------------------------------------------------------
bool AssetManager::IsValid(AssetID assetID) const
{
    return assetID != INVALID_ASSET_ID;
}



//----------------------------------------------------------------------------------------------------------------------
AssetID AssetManager::RequestAssetID()
{
	std::unique_lock<std::mutex> lock(m_assetIdMutex);
    if (m_nextAssetID == INVALID_ASSET_ID)
    {
        ERROR_AND_DIE("AssetManager::RequestAssetID - Asset ID overflowed.");
	}
    return m_nextAssetID++;
}



//----------------------------------------------------------------------------------------------------------------------
bool AssetManager::UnloadAsset(AssetID assetID)
{
    auto it = m_loadedAssets.find(assetID);
    if (it == m_loadedAssets.end())
    {
        return false;
    }

    #if defined(DEBUG_ASSET_MANAGER)
        g_devConsole->LogF(Rgba8::DarkOceanBlue, "Unloading asset: assedID(%i)", static_cast<int>(assetID));
    #endif // DEBUG_ASSET_MANAGER

    // Delete loaded asset
    delete it->second.m_asset;
    m_loadedAssets.erase(it);

    // Delete asset ID from table
    for (auto idIt = m_assetIDs.begin(); idIt != m_assetIDs.end(); ++idIt)
    {
        if (idIt->second == assetID)
        {
            #if defined(DEBUG_ASSET_MANAGER)
                g_devConsole->LogF(Rgba8::DarkOceanBlue, "- Erasing asset ID: assedID(%i)", static_cast<int>(assetID));
            #endif // DEBUG_ASSET_MANAGER
            m_assetIDs.erase(idIt);
            break;
        }
	}

    return true;
}



//----------------------------------------------------------------------------------------------------------------------
void AssetManager::LogError(Name assetName, AssetManagerError errorType) const
{
    switch (errorType)
    {
        case AssetManagerError::FailedToLoad:
            g_devConsole->LogErrorF("Asset '%s' failed to load.", assetName.ToCStr());
			break;
        case AssetManagerError::LoaderNotFound:
			g_devConsole->LogErrorF("Asset loader for '%s' not found.", assetName.ToCStr());
            break;
        case AssetManagerError::None:
            // Fallthrough
        default:
            g_devConsole->LogErrorF("Asset '%s' encountered an unknown error.", assetName.ToCStr());
			break;
    }
}



//----------------------------------------------------------------------------------------------------------------------
void AsyncLoadAssetJob::Execute()
{
    #if defined(DEBUG_ASSET_MANAGER)
        g_devConsole->LogF(Rgba8::LightOceanBlue, "- Async load job executing: %s", m_assetName.ToCStr());
    #endif // DEBUG_ASSET_MANAGER

    if (m_loaderFunc)
    {
        m_loadedAsset = m_loaderFunc(m_assetName);
        if (!m_loadedAsset)
        {
			g_assetManager->LogError(m_assetName, AssetManagerError::FailedToLoad);
        }
        m_loadedAsset->m_name = m_assetName;
	}
}



//----------------------------------------------------------------------------------------------------------------------
void AsyncLoadAssetJob::Complete()
{
    #if defined(DEBUG_ASSET_MANAGER)
        g_devConsole->LogF(Rgba8::LightOceanBlue, "- Async load job completed: %s", m_assetName.ToCStr());
    #endif // DEBUG_ASSET_MANAGER

	g_assetManager->m_loadedAssets[m_assetID].m_asset = m_loadedAsset;
	g_assetManager->m_loadedAssets[m_assetID].m_refCount = 1;
}
