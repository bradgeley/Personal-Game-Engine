﻿// Bradley Christensen - 2022-2025
#pragma once
#include "Game/Framework/EngineBuildPreferences.h"
#include "Engine/Core/EngineSubsystem.h"
#include "Engine/Core/ErrorUtils.h"
#include "Engine/Core/Name.h"
#include "Engine/Multithreading/Job.h"
#include "Asset.h"
#include "AssetKey.h"
#include "AssetLoaderFunction.h"
#include <typeindex>
#include <mutex>
#include <unordered_map>



struct NamedProperties;



//----------------------------------------------------------------------------------------------------------------------
typedef std::function<Asset* (Name)> AssetLoaderFunction;



//----------------------------------------------------------------------------------------------------------------------
// THE Asset Manager
//
extern class AssetManager* g_assetManager;



//----------------------------------------------------------------------------------------------------------------------
struct LoadedAsset
{
	LoadedAsset() = default;

    AssetKey m_key;                // Stores type information
    Asset* m_asset = nullptr;      // Pointer to the loaded asset data (e.g., GridSpriteSheet, Sound, etc.)
};



//----------------------------------------------------------------------------------------------------------------------
struct FutureAsset
{
    FutureAsset() = default;

	AssetKey m_key;                         // Stores type information
	AssetID m_assetID = AssetID::Invalid;   // Asset ID for the asset that will be loaded in the future
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

	virtual void Startup() override;
    virtual void BeginFrame() override;
    virtual void Shutdown() override;

    template<typename T>
    bool RegisterLoader(AssetLoaderFunction loader, Name debugName);

    template<typename T>
    T* Get(AssetID assetID);

    template<typename T>
    AssetID LoadSynchronous(Name assetName);

    template<typename T>
    AssetID AsyncLoad(Name assetName, int priority = 0);

    template<typename T>
    bool AsyncReload(AssetID assetID, int priority = 0);

    template<typename T>
    static AssetKey GetAssetKey(Name assetName);

    bool FindAssetKey(AssetID assetID, AssetKey& out_key) const;
    bool FindAssetKeyByTypeName(Name name, Name type, AssetKey& out_key) const;

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
    bool TryCompleteFuture(AssetID assetID, bool blocking);
    bool TryCancelOrCompleteFuture(AssetID assetID);

	AssetLoaderFunction GetLoaderFunction(std::type_index typeIndex) const;

    void LogAsyncLoadStarted(AssetKey key) const;
    void LogAsyncLoadCompleted(AssetKey key) const;
    void LogLoaded(AssetKey key) const;
    void LogUnloaded(AssetKey key) const;
    void LogError(Name assetName, AssetManagerError errorType) const;

protected:

	// Hot reload from dev console, expects 2 String parameters "type" and "name",
    // where "type" is the debug name of the asset type (e.g. "GridSpriteSheet") and "name" is the name of the asset to reload (e.g. "Data/SpriteSheets/Soldier.xml").
    static bool StaticReload(NamedProperties& params);

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
    if (!g_assetManager->IsEnabled())
    {
        return AssetID::Invalid;
    }
	return LoadSynchronousInternal(GetAssetKey<T>(assetName));
}



//----------------------------------------------------------------------------------------------------------------------
template<typename T>
inline AssetID AssetManager::AsyncLoad(Name assetName, int priority /*= 0*/)
{
    if (!g_assetManager->IsEnabled())
    {
        return AssetID::Invalid;
    }
    AssetID id = AsyncLoadInternal(GetAssetKey<T>(assetName), priority);
	ChangeRefCount(id, 1);
    return id;
}



//----------------------------------------------------------------------------------------------------------------------
template<typename T>
inline bool AssetManager::AsyncReload(AssetID assetID, int priority /*= 0*/)
{
    if (!g_assetManager->IsEnabled())
    {
        return false;
    }
	AssetKey key;
	bool found = FindAssetKey(assetID, key);
	ASSERT_OR_DIE(key.m_typeIndex == std::type_index(typeid(T)), "AssetManager::AsyncReload - Found asset type does not match the requested type.");
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
inline T* AssetManager::Get(AssetID assetID)
{
    if (m_futureAssets.find(assetID) != m_futureAssets.end())
    {
        TryCompleteFuture(assetID, false);
    }

	auto it = m_loadedAssets.find(assetID);
    if (it == m_loadedAssets.end())
    {
        return nullptr;
    }

	ASSERT_OR_DIE(it->second.m_key.m_typeIndex == std::type_index(typeid(T)), "AssetManager::Get - Asset type does not match the requested type.");
    return static_cast<T*>(it->second.m_asset);
}
