// Bradley Christensen - 2022-2025
#pragma once
#include "Engine/Core/EngineSubsystem.h"



//----------------------------------------------------------------------------------------------------------------------
// THE Asset Manager
//
extern class AssetManager* g_assetManager;



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
    
    void Startup() override;
    void BeginFrame() override;
    void Update(float deltaSeconds) override;
    void EndFrame() override;
    void Shutdown() override;
};




