// Bradley Christensen - 2022-2025
#pragma once
#include "Engine/Core/EngineSubsystem.h"
#include "Asset.h"
#include <functional>
#include <typeindex>
#include <unordered_map>



//----------------------------------------------------------------------------------------------------------------------
// THE Asset Manager
//
extern class AssetManager* g_assetManager;



//----------------------------------------------------------------------------------------------------------------------
struct AssetManagerConfig
{

};



//----------------------------------------------------------------------------------------------------------------------
struct LoadedAsset
{
    IAsset* m_asset = nullptr; // Pointer to the loaded asset data (e.g., GridSpriteSheet, Sound, etc.)
	int m_refCount = 0;     // Reference count for managing asset lifetime
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
    
    void Startup() override;
    void BeginFrame() override;
    void Update(float deltaSeconds) override;
    void EndFrame() override;
    void Shutdown() override;

	template<typename T>
    T* Get(AssetID id) const;

    void Release(AssetID id);

    bool Load(AssetID id);
    bool UnloadAsset(AssetID id);

private:

    std::unordered_map<std::type_index, std::function<IAsset*(const std::string&)>> loaders_;

    std::unordered_map<AssetID, LoadedAsset> m_loadedAssets;
};



//----------------------------------------------------------------------------------------------------------------------
template<typename T>
inline T* AssetManager::Get(AssetID id) const
{
	auto it = m_loadedAssets.find(id);
    if (it == m_loadedAssets.end())
    {
        return nullptr;
    }

    it->second.m_refCount++;
    return (T*) it->second.m_asset;
}
