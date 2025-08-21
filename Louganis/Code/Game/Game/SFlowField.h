// Bradley Christensen - 2023
#pragma once
#include "Engine/ECS/System.h"



struct Vec2;
struct IntVec2;
class FlowFieldChunk;
class FlowField;
class WorldCoords;



//----------------------------------------------------------------------------------------------------------------------
class SFlowField : public System
{
public:

    SFlowField(Name name = "FlowField", Rgba8 const& debugTint = Rgba8::Cyan) : System(name, debugTint) {};
    void Startup() override;
    void Run(SystemContext const& context) override;
    void Shutdown() override;
    int CreateMissingFlowFieldChunks(Vec2 const& anchorLocation);
    int DestroyStaleFlowFieldChunks(Vec2 const& anchorLocation);

private:
    
    void GenerateFlow(FlowField& flowField, WorldCoords const& destination);
    void GenerateDistanceField(FlowField& flowField);
    void GenerateGradient(FlowField& flowField);
};
