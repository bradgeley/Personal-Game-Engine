// Bradley Christensen - 2022-2025
#pragma once
#include "Engine/Core/EngineSubsystem.h"
#include "Engine/Core/Name.h"
#include "Asset.h"
#include "AssetKey.h"
#include "AssetLoaders.h"
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
    IAsset* m_asset = nullptr; // Pointer to the loaded asset data (e.g., GridSpriteSheet, Sound, etc.)
	int m_refCount = 0;     // Reference count for managing asset lifetime
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
public:
    
    explicit AssetManager(AssetManagerConfig const& config);
    
    //void Startup() override;
    //void BeginFrame() override;
    //void Update(float deltaSeconds) override;
    //void EndFrame() override;
    //void Shutdown() override;

	template<typename T>
    bool RegisterLoader(AssetLoaderFunction loader);

	template<typename T>
    T* Get(AssetID assetID) const;

    template<typename T>
    AssetID Load(Name assetName);

    template<typename T>
    static AssetKey GetAssetKey(Name assetName);

    void Release(AssetID assetID);
	bool IsValid(AssetID assetID) const;

	AssetID RequestAssetID();

protected:

    void LogError(Name assetName, AssetManagerError errorType);

protected:

    AssetManagerConfig const m_config;

	std::mutex m_assetIdMutex;
	AssetID m_nextAssetID = 0;

    std::unordered_map<std::type_index, AssetLoaderFunction> m_loaderFuncs;

    std::unordered_map<AssetKey, AssetID, AssetKeyHash> m_assetIDs;
    std::unordered_map<AssetID, LoadedAsset> m_loadedAssets;
};



//----------------------------------------------------------------------------------------------------------------------
template<typename T>
inline AssetID AssetManager::Load(Name assetName)
{
	AssetKey assetKey = GetAssetKey<T>(assetName);
	auto assetID_it = m_assetIDs.find(assetKey);
    if (assetID_it != m_assetIDs.end())
    {
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
