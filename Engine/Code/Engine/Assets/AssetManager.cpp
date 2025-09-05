// Bradley Christensen - 2022-2025
#include "AssetManager.h"
#include "AsyncLoadAssetJob.h"
#include "Engine/Core/EngineCommon.h"
#include "Engine/Core/ErrorUtils.h"
#include "Engine/Assets/Sprites/GridSpriteSheet.h"
#include "Engine/Assets/Sprites/SpriteAnimation.h"
#include "Engine/Debug/DevConsole.h"
#include "Engine/Multithreading/Jobsystem.h"



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
bool AssetManager::FindAssetKey(AssetID assetID, AssetKey& out_key) const
{
	// Search loaded assets first
    {
        auto loadedAssetIt = m_loadedAssets.find(assetID);
        if (loadedAssetIt != m_loadedAssets.end())
        {
            out_key = loadedAssetIt->second.m_key;
            return true;
        }
    }

    // Then future assets
    {
        auto futureIt = m_futureAssets.find(assetID);
        if (futureIt != m_futureAssets.end())
        {
            out_key = futureIt->second.m_key;
            return true;
        }
    }

    return false;
}



//----------------------------------------------------------------------------------------------------------------------
void AssetManager::Release(AssetID assetID)
{
    ChangeRefCount(assetID, -1);

    if (GetRefCount(assetID) <= 0 && IsFuture(assetID))
    {
        TryCancelOrCompleteFuture(assetID);
    }

    if (!IsLoaded(assetID))
    {
        return;
    }

    if (GetRefCount(assetID) <= 0)
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
bool AssetManager::IsLoaded(AssetID assetID) const
{
    if (!IsValid(assetID))
    {
        return false;
	}
	return m_loadedAssets.find(assetID) != m_loadedAssets.end();
}



//----------------------------------------------------------------------------------------------------------------------
bool AssetManager::IsFuture(AssetID assetID) const
{
    if (!IsValid(assetID))
    {
        return false;
    }
	return m_futureAssets.find(assetID) != m_futureAssets.end();
}



//----------------------------------------------------------------------------------------------------------------------
bool AssetManager::TryCancelAsyncLoad(AssetID assetID)
{
	auto futureIt = m_futureAssets.find(assetID);
    if (futureIt == m_futureAssets.end())
    {
        return false;
	}

	bool cancelled = g_jobSystem->TryCancelLoadingJob(futureIt->second.m_jobID);
    return cancelled;
}



//----------------------------------------------------------------------------------------------------------------------
uint32_t AssetManager::GetRefCount(AssetID assetID) const
{
	auto it = m_refCounts.find(assetID);
    if (it != m_refCounts.end())
    {
        return it->second;
	}
    return 0;
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
AssetID AssetManager::GetAssetID(AssetKey const& key) const
{
	auto assetID_it = m_assetIDs.find(key);
    if (assetID_it != m_assetIDs.end())
    {
        return assetID_it->second;
    }
	return INVALID_ASSET_ID;
}



//----------------------------------------------------------------------------------------------------------------------
AssetID AssetManager::LoadSynchronousInternal(Name assetName, AssetKey key)
{
	AssetID assetID = GetAssetID(key);

    if (IsFuture(assetID))
    {
		// May complete the load here if the asset is already being async loaded
        TryCancelOrCompleteFuture(assetID);
    }

    if (IsLoaded(assetID))
    {
        return assetID;
    }

    auto it = m_loaderFuncs.find(key.m_typeIndex);
    if (it == m_loaderFuncs.end())
    {
        LogError(assetName, AssetManagerError::LoaderNotFound);
		return INVALID_ASSET_ID;
    }

    AssetLoaderFunction loader = it->second;

    // ~Actually load the asset~
    IAsset* asset = loader(assetName);

    if (!asset)
    {
        LogError(assetName, AssetManagerError::FailedToLoad);
        return INVALID_ASSET_ID;
    }

    assetID = RequestAssetID();
    m_assetIDs[key] = assetID;

    asset->m_assetID = assetID;
    asset->m_name = assetName;

    LoadedAsset& loaded = m_loadedAssets[assetID];
    loaded.m_asset = asset;
    loaded.m_key = key;
    ChangeRefCount(assetID, 1);
    return assetID;
}



//----------------------------------------------------------------------------------------------------------------------
AssetID AssetManager::AsyncLoadInternal(Name assetName, AssetKey key)
{
    if (!g_jobSystem)
    {
        return LoadSynchronousInternal(assetName, key);
    }

    AssetID assetID = GetAssetID(key);

    auto it = m_loaderFuncs.find(key.m_typeIndex);
    if (it != m_loaderFuncs.end())
    {
        AssetID assetID = RequestAssetID();
        m_assetIDs[key] = assetID;

        AsyncLoadAssetJob* loadJob = new AsyncLoadAssetJob();
        loadJob->m_assetName = assetName;
        loadJob->m_assetID = assetID;
        loadJob->m_loaderFunc = it->second;
        loadJob->SetPriority(0);

        JobID jobID = g_jobSystem->PostLoadingJob(loadJob);

        FutureAsset futureAsset(key);
        futureAsset.m_jobID = jobID;
        futureAsset.m_assetID = assetID;
        m_futureAssets.emplace(futureAsset.m_assetID, futureAsset);

        return futureAsset.m_assetID;
    }

    LogError(assetName, AssetManagerError::LoaderNotFound);
    return INVALID_ASSET_ID;
}



//----------------------------------------------------------------------------------------------------------------------
AssetID AssetManager::AsyncReloadInternal(AssetID assetID, AssetKey key)
{
    if (m_futureAssets.find(assetID) != m_futureAssets.end())
    {
        return false;
    }

    auto loadedAssetIt = m_loadedAssets.find(assetID);
    if (loadedAssetIt == m_loadedAssets.end())
    {
        return false;
    }

    Name loadedAssetName = key.m_name;

    // Delete loaded asset
    delete loadedAssetIt->second.m_asset;
    m_loadedAssets.erase(loadedAssetIt);

    // Async load without creating a new asset ID
    auto it = m_loaderFuncs.find(key.m_typeIndex);
    if (it != m_loaderFuncs.end())
    {
        AsyncLoadAssetJob* loadJob = new AsyncLoadAssetJob();
        loadJob->m_assetName = loadedAssetName;
        loadJob->m_assetID = assetID;
        loadJob->m_loaderFunc = it->second;
        loadJob->SetPriority(0);

        JobID jobID = g_jobSystem->PostLoadingJob(loadJob);

        FutureAsset futureAsset(key);
        futureAsset.m_jobID = jobID;
        futureAsset.m_assetID = assetID;
        m_futureAssets.emplace(futureAsset.m_assetID, futureAsset);

        return true;
    }

    return false;
}



//----------------------------------------------------------------------------------------------------------------------
void AssetManager::ChangeRefCount(AssetID assetID, int32_t delta)
{
    if (delta > 0)
    {
		m_refCounts[assetID] += delta;
    }
    else if (delta < 0)
    {
        auto it = m_refCounts.find(assetID);
        if (it != m_refCounts.end())
        {
            it->second += delta;
        }
    }
}



//----------------------------------------------------------------------------------------------------------------------
bool AssetManager::UnloadAsset(AssetID assetID)
{
    if (!IsLoaded(assetID))
    {
        return false;
    }

	ASSERT_OR_DIE(m_futureAssets.find(assetID) == m_futureAssets.end(), "AssetManager::UnloadAsset - This should be called after removing the asset from future.");

    // Delete loaded asset
    delete m_loadedAssets.at(assetID).m_asset;
    m_loadedAssets.erase(assetID);

    // Delete asset ID from table
    for (auto idIt = m_assetIDs.begin(); idIt != m_assetIDs.end(); ++idIt)
    {
        if (idIt->second == assetID)
        {
            m_assetIDs.erase(idIt);
            break;
        }
	}
    
	// Delete ref count entry
	m_refCounts.erase(assetID);

    return true;
}



//----------------------------------------------------------------------------------------------------------------------
bool AssetManager::TryCancelOrCompleteFuture(AssetID assetID)
{
    bool completed = false;
    auto futureIt = m_futureAssets.find(assetID);
    if (futureIt != m_futureAssets.end())
    {
        bool cancelled = g_jobSystem->TryCancelLoadingJob(futureIt->second.m_jobID);
        if (!cancelled)
        {
            completed = g_jobSystem->CompleteJob(futureIt->second.m_jobID, true);
        }
        m_futureAssets.erase(futureIt);
    }
    return completed;
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
