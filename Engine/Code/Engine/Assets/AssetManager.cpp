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
    auto it = m_loadedAssets.find(assetID);
    if (it == m_loadedAssets.end())
    {
        return;
    }

    it->second.m_refCount--;

    if (it->second.m_refCount <= 0)
    {
        delete it->second.m_asset;
		m_loadedAssets.erase(it);
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
        g_devConsole->LogSuccessF("- Async load job executing: %s", m_assetName.ToCStr());
    #endif // DEBUG_ASSET_MANAGER

    if (m_loaderFunc)
    {
        m_loadedAsset = m_loaderFunc(m_assetName);
	}
}



//----------------------------------------------------------------------------------------------------------------------
void AsyncLoadAssetJob::Complete()
{
    #if defined(DEBUG_ASSET_MANAGER)
        g_devConsole->LogSuccessF("- Async load job completing: %s", m_assetName.ToCStr());
    #endif // DEBUG_ASSET_MANAGER

	g_assetManager->m_loadedAssets[m_assetID].m_asset = m_loadedAsset;
	g_assetManager->m_loadedAssets[m_assetID].m_refCount = 1;
}
