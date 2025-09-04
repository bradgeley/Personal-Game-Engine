// Bradley Christensen - 2022-2025
#pragma once
#include "Game/Framework/EngineBuildPreferences.h"
#include "Engine/Core/EngineSubsystem.h"
#include "Engine/Core/Name.h"
#include "Engine/Multithreading/Job.h"
#include "Engine/Multithreading/Jobsystem.h"
#include "Asset.h"
#include "AssetKey.h"
#include "AssetLoaders.h"
#include <typeindex>
#include <mutex>
#include <unordered_map>

#if defined(DEBUG_ASSET_MANAGER)
#include "Engine/Core/StringUtils.h"
#include "Engine/Debug/DevConsole.h"
#endif // DEBUG_ASSET_MANAGER



//----------------------------------------------------------------------------------------------------------------------
typedef std::function<IAsset* (Name)> AssetLoaderFunction;



//----------------------------------------------------------------------------------------------------------------------
// THE Asset Manager
//
extern class AssetManager* g_assetManager;



//----------------------------------------------------------------------------------------------------------------------
struct LoadedAsset
{
    IAsset* m_asset = nullptr;      // Pointer to the loaded asset data (e.g., GridSpriteSheet, Sound, etc.)
	int m_refCount = 0;             // Reference count for managing asset lifetime
};



//----------------------------------------------------------------------------------------------------------------------
struct FutureAsset
{
	AssetID m_assetID = INVALID_ASSET_ID;   // Asset ID for the asset that will be loaded in the future
	JobID m_jobID = JobID::Invalid;         // Job ID for the loading job associated with this asset
};



//----------------------------------------------------------------------------------------------------------------------
class AsyncLoadAssetJob : public Job
{
public:

    void Execute() override;
    void Complete() override;

public:

    Name m_assetName;
    AssetID m_assetID;
    IAsset* m_loadedAsset = nullptr;
    AssetLoaderFunction m_loaderFunc;
};



//----------------------------------------------------------------------------------------------------------------------
enum class AssetManagerError
{
    None,
    LoaderNotFound,
    FailedToLoad
};



//----------------------------------------------------------------------------------------------------------------------
struct AssetManagerConfig
{

};



//----------------------------------------------------------------------------------------------------------------------
// Asset Manager
//
// Responsible for loading, unloading, and managing game assets (sprite sheets, etc.) that are not directly owned by other systems
// Textures are owned by the Renderer, sounds by the Audio system, etc.
//
class AssetManager : public EngineSubsystem
{
	friend class AsyncLoadAssetJob;

public:
    
    explicit AssetManager(AssetManagerConfig const& config);
    
    //void Startup() override;
    void BeginFrame() override;
    //void Update(float deltaSeconds) override;
    //void EndFrame() override;
    //void Shutdown() override;

	template<typename T>
    bool RegisterLoader(AssetLoaderFunction loader);

	template<typename T>
    T* Get(AssetID assetID) const;

    template<typename T>
    AssetID LoadSynchronous(Name assetName);

    template<typename T>
    AssetID AsyncLoad(Name assetName);

    template<typename T>
    static AssetKey GetAssetKey(Name assetName);

    void Release(AssetID assetID);
	bool IsValid(AssetID assetID) const;

	AssetID RequestAssetID();

protected:

    void LogError(Name assetName, AssetManagerError errorType) const;

protected:

    AssetManagerConfig const m_config;

	std::mutex m_assetIdMutex;
	AssetID m_nextAssetID = 0;

    std::unordered_map<std::type_index, AssetLoaderFunction> m_loaderFuncs;

    std::unordered_map<AssetKey, AssetID, AssetKeyHash> m_assetIDs;
    std::unordered_map<AssetID, LoadedAsset> m_loadedAssets;
    std::unordered_map<AssetID, FutureAsset> m_futureAssets;
};



//----------------------------------------------------------------------------------------------------------------------
template<typename T>
inline AssetID AssetManager::LoadSynchronous(Name assetName)
{
    #if defined(DEBUG_ASSET_MANAGER)
        g_devConsole->AddLine(StringUtils::StringF("AssetManager::LoadSynchronous: %s", assetName.ToCStr()));
	#endif // DEBUG_ASSET_MANAGER

	AssetKey assetKey = GetAssetKey<T>(assetName);
	auto assetID_it = m_assetIDs.find(assetKey);
    if (assetID_it != m_assetIDs.end())
    {
        #if defined(DEBUG_ASSET_MANAGER)
            g_devConsole->AddLine(StringUtils::StringF("- Asset already loaded: %s", assetName.ToCStr()));
        #endif // DEBUG_ASSET_MANAGER
        return assetID_it->second;
    }

	auto it = m_loaderFuncs.find(std::type_index(typeid(T)));
    if (it != m_loaderFuncs.end())
    {
        AssetLoaderFunction loader = it->second;
        IAsset* asset = loader(assetName);
        if (!asset)
        {
            LogError(assetName, AssetManagerError::FailedToLoad);
            return INVALID_ASSET_ID;
        }

        AssetID assetID = RequestAssetID();

        #if defined(DEBUG_ASSET_MANAGER)
            g_devConsole->LogSuccessF("- Asset loaded: %s id(%i)", assetName.ToCStr(), static_cast<int>(assetID));
        #endif // DEBUG_ASSET_MANAGER

        LoadedAsset& loaded = m_loadedAssets[assetID];
        loaded.m_asset = asset;
        loaded.m_refCount = 1;
        return assetID;
	}

    LogError(assetName, AssetManagerError::LoaderNotFound);
    return INVALID_ASSET_ID;
}



//----------------------------------------------------------------------------------------------------------------------
template<typename T>
inline AssetID AssetManager::AsyncLoad(Name assetName)
{
    #if defined(DEBUG_ASSET_MANAGER)
        g_devConsole->AddLine(StringUtils::StringF("AssetManager::AsyncLoad: %s", assetName.ToCStr()));
    #endif // DEBUG_ASSET_MANAGER

    if (!g_jobSystem)
    {
		return LoadSynchronous<T>(assetName);
    }

    AssetKey assetKey = GetAssetKey<T>(assetName);
    auto assetID_it = m_assetIDs.find(assetKey);
    if (assetID_it != m_assetIDs.end())
    {
        #if defined(DEBUG_ASSET_MANAGER)
            g_devConsole->AddLine(StringUtils::StringF("- Asset already loaded: %s", assetName.ToCStr()));
        #endif // DEBUG_ASSET_MANAGER
        return assetID_it->second;
    }

    auto it = m_loaderFuncs.find(std::type_index(typeid(T)));
    if (it != m_loaderFuncs.end())
    {
        AssetID futureAssetID = RequestAssetID();

        AsyncLoadAssetJob* loadJob = new AsyncLoadAssetJob();
		loadJob->m_assetName = assetName;
		loadJob->m_assetID = futureAssetID;
        loadJob->m_loaderFunc = it->second;
        loadJob->SetPriority(0);

        JobID jobID = g_jobSystem->PostLoadingJob(loadJob);

        #if defined(DEBUG_ASSET_MANAGER)
            g_devConsole->LogSuccessF("- Async load job posted: %s assetID(%i) jobID(%i)", assetName.ToCStr(), static_cast<int>(futureAssetID), static_cast<int>(jobID.m_uniqueID));
        #endif // DEBUG_ASSET_MANAGER

        FutureAsset futureAsset;
        futureAsset.m_jobID = jobID;
        futureAsset.m_assetID = futureAssetID;
		m_futureAssets.emplace(futureAsset.m_assetID, futureAsset);

        return futureAsset.m_assetID;
    }

    LogError(assetName, AssetManagerError::LoaderNotFound);
    return INVALID_ASSET_ID;
}



//----------------------------------------------------------------------------------------------------------------------
template<typename T>
inline AssetKey AssetManager::GetAssetKey(Name assetName)
{
    return AssetKey(assetName, std::type_index(typeid(T)));
}



//----------------------------------------------------------------------------------------------------------------------
template<typename T>
inline bool AssetManager::RegisterLoader(AssetLoaderFunction loader)
{
	std::type_index typeIndex(typeid(T));
    if (m_loaderFuncs.find(typeIndex) != m_loaderFuncs.end())
    {
        // Loader for this type already exists
        return false;
	}

	m_loaderFuncs.emplace(typeIndex, loader);
    return true;
}



//----------------------------------------------------------------------------------------------------------------------
template<typename T>
inline T* AssetManager::Get(AssetID assetID) const
{
	auto it = m_loadedAssets.find(assetID);
    if (it == m_loadedAssets.end())
    {
        return nullptr;
    }

    return static_cast<T*>(it->second.m_asset);
}
