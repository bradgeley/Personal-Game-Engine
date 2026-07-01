// Bradley Christensen - 2022-2026
#pragma once
#include "Engine/ECS/System.h"



struct Vec2;
struct IntVec2;
class FlowFieldChunk;
class FlowField;
class SCWorld;
class WorldCoords;



//----------------------------------------------------------------------------------------------------------------------
class SFlowField : public System
{
public:

    SFlowField(Name name = "FlowField", Rgba8 const& debugTint = Rgba8::Cyan) : System(name, debugTint) {};
    void Startup() override;
    void Shutdown() const override;
    void Run(SystemContext const& context) const override;

    static void SeedFlowField(FlowField& flowField, SCWorld const& world);
    static void SetCostField(FlowField& flowField, SCWorld const& world);
    static void GenerateFlow(FlowField& flowField, SCWorld const& world);
    static void GenerateDistanceField(FlowField& flowField, SCWorld const& world);
    static void GenerateGradient(FlowField& flowField, SCWorld const& world);
};
