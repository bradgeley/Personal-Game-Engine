// Bradley Christensen - 2023
#pragma once
#include "Engine/ECS/System.h"



struct IntVec2;
class FlowFieldChunk;
class FlowField;
class WorldCoords;



//----------------------------------------------------------------------------------------------------------------------
class SFlowField : public System
{
public:

    SFlowField(std::string const& name = "FlowField", Rgba8 const& debugTint = Rgba8::DarkRed) : System(name, debugTint) {};
    void Startup() override;
    void Run(SystemContext const& context) override;
    void Shutdown() override;
    int CreateMissingFlowFieldChunks();
    int DestroyStaleFlowFieldChunks();

private:
    
    void GenerateFlow(FlowField& flowField, WorldCoords const& destination);
    void GenerateDistanceField(FlowField& flowField);
    void GenerateGradient(FlowField& flowField);
};
