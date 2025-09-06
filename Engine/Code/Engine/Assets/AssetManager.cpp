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
AssetManager::~AssetManager()
{
    g_assetManager = nullptr;
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
void AssetManager::Shutdown()
{
    while (!m_futureAssets.empty())
    {
        TryCancelOrCompleteFuture(m_futureAssets.begin()->first);
	}

    while (m_loadedAssets.size() > 0)
    {
        UnloadAsset(m_loadedAssets.begin()->first);
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
AssetID AssetManager::LoadSynchronousInternal(AssetKey key)
{
	AssetID assetID = GetAssetID(key);

    if (IsValid(assetID))
    {
        // If the asset ID exists already, this asset is either loaded or future
        if (IsFuture(assetID))
        {
            TryCancelOrCompleteFuture(assetID);
        }

		// At this point, either the asset was just loaded from the future, or it was already loaded
        // If not loaded, we must have just cancelled the future, so fallthrough and load
        if (IsLoaded(assetID))
        {
            ChangeRefCount(assetID, 1);
            return assetID;
        }
	}

    // At this point we know the asset is not loaded or future

	AssetLoaderFunction loader = GetLoaderFunction(key.m_typeIndex);
    if (!loader)
    {
        LogError(key.m_name, AssetManagerError::LoaderNotFound);
		return INVALID_ASSET_ID;
    }

    // ~Actually load the asset~
    IAsset* asset = loader(key.m_name);

    if (!asset)
    {
        LogError(key.m_name, AssetManagerError::FailedToLoad);
        return INVALID_ASSET_ID;
    }

    // ~Complete the load~
    bool succeeded = asset->CompleteSyncLoad();
    if (!succeeded)
    {
        asset->ReleaseResources();
        m_loadedAssets.erase(assetID);
		return INVALID_ASSET_ID;
    }

	LogLoaded(key);

    if (!IsValid(assetID))
    {
        // The only cases where assetID could already be valid at this point is if it was a future that we cancelled in order to load now,
        // or if we are reloading the asset
        assetID = RequestAssetID();
        m_assetIDs[key] = assetID;
    }

    asset->m_assetID = assetID;
    asset->m_name = key.m_name;

    LoadedAsset& loaded = m_loadedAssets[assetID];
    loaded.m_asset = asset;
    loaded.m_key = key;

    ChangeRefCount(assetID, 1);
    return assetID;
}



//----------------------------------------------------------------------------------------------------------------------
AssetID AssetManager::AsyncLoadInternal(AssetKey key, int priority /*= 0*/)
{
    AssetID assetID = GetAssetID(key);
    if (IsValid(assetID))
    {
        // Either loaded or future
        return assetID;
    }

    if (!g_jobSystem)
    {
        return LoadSynchronousInternal(key);
    }

	AssetLoaderFunction loader = GetLoaderFunction(key.m_typeIndex);
    if (!loader)
    {
		LogError(key.m_name, AssetManagerError::LoaderNotFound);
		return INVALID_ASSET_ID;
    }

    assetID = RequestAssetID();
    m_assetIDs[key] = assetID;

    AsyncLoadAssetJob* loadJob = new AsyncLoadAssetJob();
    loadJob->m_assetKey = key;
    loadJob->m_assetID = assetID;
    loadJob->m_loaderFunc = loader;
    loadJob->SetPriority(priority);

    JobID jobID = g_jobSystem->PostLoadingJob(loadJob);

    FutureAsset futureAsset;
    futureAsset.m_jobID = jobID;
    futureAsset.m_assetID = assetID;
    futureAsset.m_key = key;
    m_futureAssets.emplace(assetID, futureAsset);

    return futureAsset.m_assetID;
}



//----------------------------------------------------------------------------------------------------------------------
AssetID AssetManager::AsyncReloadInternal(AssetID assetID, AssetKey key, int priority /*= 0*/)
{
    ASSERT_OR_DIE(IsValid(assetID) && (GetAssetID(key) == assetID), "AssetManager::AsyncReloadInternal - trying to reload an invalid assetID.");

    if (IsFuture(assetID))
    {
        // Already in the process of being async loaded, no need to restart it? or should we try to cancel and restart?
        return assetID;
    }

    // At this point we know the asset is already loaded
	ASSERT_OR_DIE(IsLoaded(assetID), "AssetManager::AsyncReloadInternal - AssetID was valid, but asset was not future or loaded.");

	UnloadAsset(assetID, true);

    if (!g_jobSystem)
    {
		return LoadSynchronousInternal(key);
    }

	AssetLoaderFunction loader = GetLoaderFunction(key.m_typeIndex);
    if (!loader)
    {
        LogError(key.m_name, AssetManagerError::LoaderNotFound);
		return INVALID_ASSET_ID;
    }

    AsyncLoadAssetJob* loadJob = new AsyncLoadAssetJob();
    loadJob->m_assetKey = key;
    loadJob->m_assetID = assetID;
    loadJob->m_loaderFunc = loader;
    loadJob->SetPriority(priority);

    JobID jobID = g_jobSystem->PostLoadingJob(loadJob);

    FutureAsset futureAsset;
    futureAsset.m_jobID = jobID;
    futureAsset.m_assetID = assetID;
	futureAsset.m_key = key;
    m_futureAssets.emplace(assetID, futureAsset);

    return true;
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
bool AssetManager::UnloadAsset(AssetID assetID, bool isReloading /*= false*/)
{
	ASSERT_OR_DIE(!IsFuture(assetID), "AssetManager::UnloadAsset - This should be called after removing the asset from future.");
	ASSERT_OR_DIE(IsLoaded(assetID), "AssetManager::UnloadAsset - This should only be called if the asset is already loaded.");

	AssetKey key = m_loadedAssets.at(assetID).m_key;

    // Delete loaded asset
    delete m_loadedAssets.at(assetID).m_asset;
    m_loadedAssets.erase(assetID);

	LogUnloaded(key);

    // Delete asset ID from table, unless doing a reload in which case we want to keep the asset ID and ref count
    if (!isReloading)
    {
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
    }

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
AssetLoaderFunction AssetManager::GetLoaderFunction(std::type_index typeIndex) const
{
	auto loaderFuncIt = m_loaderFuncs.find(typeIndex);
    if (loaderFuncIt != m_loaderFuncs.end())
    {
        return loaderFuncIt->second;
	}
    return nullptr;
}



//----------------------------------------------------------------------------------------------------------------------
void AssetManager::LogLoaded(AssetKey key) const
{
    if (!g_devConsole)
    {
        return;
    }

    #if defined(DEBUG_ASSET_MANAGER)
        auto debugNameIt = m_loaderDebugNames.find(key.m_typeIndex);
        const char* debugNameStr = (debugNameIt != m_loaderDebugNames.end()) ? debugNameIt->second.ToCStr() : "Unknown Asset Type";
        g_devConsole->LogF(Rgba8::LightOceanBlue, "Loaded %s: '%s' successfully.", debugNameStr, key.m_name.ToCStr());
	#endif // defined(DEBUG_ASSET_MANAGER)
}



//----------------------------------------------------------------------------------------------------------------------
void AssetManager::LogUnloaded(AssetKey key) const
{
    if (!g_devConsole)
    {
        return;
    }

	#if defined(DEBUG_ASSET_MANAGER)
        auto debugNameIt = m_loaderDebugNames.find(key.m_typeIndex);
        const char* debugNameStr = (debugNameIt != m_loaderDebugNames.end()) ? debugNameIt->second.ToCStr() : "Unknown Asset Type";
        g_devConsole->LogF(Rgba8::DarkOceanBlue, "Unloaded %s: '%s' successfully.", debugNameStr, key.m_name.ToCStr());
	#endif // defined(DEBUG_ASSET_MANAGER)
}



//----------------------------------------------------------------------------------------------------------------------
void AssetManager::LogError(Name assetName, AssetManagerError errorType) const
{
    if (!g_devConsole)
    {
        return;
    }
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