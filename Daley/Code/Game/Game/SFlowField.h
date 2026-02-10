// Bradley Christensen - 2022-2025
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
    void Run(SystemContext const& context) override;
    void Shutdown() override;

private:

    void SeedFlowField(FlowField& flowField, SCWorld const& world);
    void SetCostField(FlowField& flowField, SCWorld const& world);
    void GenerateFlow(FlowField& flowField);
    void GenerateDistanceField(FlowField& flowField);
    void GenerateGradient(FlowField& flowField);
};
