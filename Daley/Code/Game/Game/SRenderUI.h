// Bradley Christensen - 2022-2026
#pragma once
#include "Engine/ECS/System.h"



struct TowerPlacementRequest;
struct TowerSwirlRequest;



//----------------------------------------------------------------------------------------------------------------------
class SRenderUI : public System
{
public:

    SRenderUI(Name name = "RenderUI", Rgba8 const& debugTint = Rgba8::Gold) : System(name, debugTint) {};
    void Startup() override;
    void Shutdown() const override;
    void Run(SystemContext const& context) const override;

private:

	void RenderTowerPlacementPreview(SystemContext const& context, TowerPlacementRequest const& placementInfo) const;
	void RenderSwirlPreview(SystemContext const& context, TowerSwirlRequest const& placementInfo) const;
};
