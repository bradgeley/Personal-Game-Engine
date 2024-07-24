// Bradley Christensen - 2023
#pragma once
#include "Engine/ECS/System.h"



struct IntVec2;
class FlowFieldChunk;



//----------------------------------------------------------------------------------------------------------------------
class SFlowField : public System
{
public:

    SFlowField(std::string const& name = "FlowField") : System(name) {};
    void Startup() override;
    void Run(SystemContext const& context) override;

private:
    
    void GenerateChunk(FlowFieldChunk* chunk, IntVec2 const& seedLocalTileCoords);
    void GenerateCostField(FlowFieldChunk* chunk);
    void SeedDistanceField(FlowFieldChunk* chunk, IntVec2 const& seedLocalTileCoords);
    void GenerateDistanceField(FlowFieldChunk* chunk);
    void GenerateGradient(FlowFieldChunk* chunk);
};
