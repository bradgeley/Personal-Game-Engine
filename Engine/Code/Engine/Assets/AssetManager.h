// Bradley Christensen - 2022-2025
#pragma once
#include "Game/Framework/EngineBuildPreferences.h"
#include "Engine/Core/EngineSubsystem.h"
#include "Engine/Core/Name.h"
#include "Engine/Multithreading/Job.h"
#include "Asset.h"
#include "AssetKey.h"
#include "AssetLoaderFunction.h"
#include <typeindex>
#include <mutex>
#include <unordered_map>



//----------------------------------------------------------------------------------------------------------------------
typedef std::function<IAsset* (Name)> AssetLoaderFunction;



//----------------------------------------------------------------------------------------------------------------------
// THE Asset Manager
//
extern class AssetManager* g_assetManager;



//----------------------------------------------------------------------------------------------------------------------
struct LoadedAsset
{
	LoadedAsset() = default;

    AssetKey m_key;                 // Stores type information
    IAsset* m_asset = nullptr;      // Pointer to the loaded asset data (e.g., GridSpriteSheet, Sound, etc.)
};



//----------------------------------------------------------------------------------------------------------------------
struct FutureAsset
{
    FutureAsset() = default;

	AssetKey m_key;                         // Stores type information
	AssetID m_assetID = INVALID_ASSET_ID;   // Asset ID for the asset that will be loaded in the future
	JobID m_jobID = JobID::Invalid;         // Job ID for the loading job associated with this asset
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
    virtual ~AssetManager() override;

    virtual void BeginFrame() override;
    virtual void Shutdown() override;

    template<typename T>
    bool RegisterLoader(AssetLoaderFunction loader, Name debugName);

    template<typename T>
    T* Get(AssetID assetID) const;

    template<typename T>
    AssetID LoadSynchronous(Name assetName);

    template<typename T>
    AssetID AsyncLoad(Name assetName, int priority = 0);

    template<typename T>
    bool AsyncReload(AssetID assetID, int priority = 0);

    template<typename T>
    static AssetKey GetAssetKey(Name assetName);

    bool FindAssetKey(AssetID assetID, AssetKey& out_key) const;

    void Release(AssetID assetID);
    bool IsValid(AssetID assetID) const;
    bool IsLoaded(AssetID assetID) const;
    bool IsFuture(AssetID assetID) const;
    bool TryCancelAsyncLoad(AssetID assetID);
	uint32_t GetRefCount(AssetID assetID) const;

protected:

    AssetID RequestAssetID();
    AssetID GetAssetID(AssetKey const& key) const;

    AssetID LoadSynchronousInternal(AssetKey key);
    AssetID AsyncLoadInternal(AssetKey key, int priority = 0);
    AssetID AsyncReloadInternal(AssetID assetID, AssetKey key, int priority = 0);

    void ChangeRefCount(AssetID assetID, int32_t delta);
    bool UnloadAsset(AssetID assetID, bool isReloading = false);
    bool TryCancelOrCompleteFuture(AssetID assetID);

	AssetLoaderFunction GetLoaderFunction(std::type_index typeIndex) const;

    void LogLoaded(AssetKey key) const;
    void LogUnloaded(AssetKey key) const;
    void LogError(Name assetName, AssetManagerError errorType) const;

protected:

    AssetManagerConfig const m_config;

    std::mutex m_assetIdMutex;
    AssetID m_nextAssetID = 0;

    std::unordered_map<std::type_index, AssetLoaderFunction> m_loaderFuncs;
    std::unordered_map<std::type_index, Name> m_loaderDebugNames;

    std::unordered_map<AssetKey, AssetID, AssetKeyHash> m_assetIDs; // Prevents trying to load the same asset multiple times (1 asset per name+type, e.g. Soldier.xml as a GridSpriteSheet)
    std::unordered_map<AssetID, LoadedAsset> m_loadedAssets;
    std::unordered_map<AssetID, FutureAsset> m_futureAssets;
    std::unordered_map<AssetID, uint32_t>    m_refCounts;           // ref count for both loaded and future assets
};


//----------------------------------------------------------------------------------------------------------------------
// Guaranteed to return the loaded asset if it exists.
//
template<typename T>
inline AssetID AssetManager::LoadSynchronous(Name assetName)
{
	return LoadSynchronousInternal(GetAssetKey<T>(assetName));
}



//----------------------------------------------------------------------------------------------------------------------
template<typename T>
inline AssetID AssetManager::AsyncLoad(Name assetName, int priority /*= 0*/)
{
    return AsyncLoadInternal(GetAssetKey<T>(assetName), priority);
}



//----------------------------------------------------------------------------------------------------------------------
template<typename T>
inline bool AssetManager::AsyncReload(AssetID assetID, int priority /*= 0*/)
{
	AssetKey key;
	bool found = FindAssetKey(assetID, key);
    if (found)
    {
		AsyncReloadInternal(assetID, key, priority);
    }
    return false;
}



//----------------------------------------------------------------------------------------------------------------------
template<typename T>
inline AssetKey AssetManager::GetAssetKey(Name assetName)
{
    return AssetKey(assetName, std::type_index(typeid(T)));
}



//----------------------------------------------------------------------------------------------------------------------
template<typename T>
inline bool AssetManager::RegisterLoader(AssetLoaderFunction loader, Name debugName)
{
	std::type_index typeIndex(typeid(T));
    if (m_loaderFuncs.find(typeIndex) != m_loaderFuncs.end())
    {
        // Loader for this type already exists
        return false;
	}

	m_loaderDebugNames.emplace(typeIndex, debugName);
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
