// Bradley Christensen - 2022-2026
#pragma once
#include "Engine/ECS/System.h"



struct SCEntityFactory;
struct TowerPlacementInfo;
class SCWorld;



//----------------------------------------------------------------------------------------------------------------------
enum class TowerPlacementResult
{
    Success,
    Blocked,
    BlocksPath,
};



//----------------------------------------------------------------------------------------------------------------------
class STowerSpawner : public System
{
public:

    STowerSpawner(Name name = "TowerSpawner", Rgba8 const& debugTint = Rgba8::Gray) : System(name, debugTint) {};
    void Startup() override;
    void Shutdown() const override;
    void Run(SystemContext const& context) const override;

    TowerPlacementResult CanPlaceTower(TowerPlacementInfo const& info, SCEntityFactory& factory, SCWorld const& world) const;
    bool PlaceTowerInWorld(TowerPlacementInfo const& placementInfo, SCWorld& world) const;
};
