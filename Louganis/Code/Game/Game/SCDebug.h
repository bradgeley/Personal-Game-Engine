// Bradley Christensen - 2024
#pragma once
#include "Engine/Core/EngineSubsystem.h"
#include "WorldSettings.h"
#include <vector>



class VertexBuffer;
class Chunk;



//----------------------------------------------------------------------------------------------------------------------
class SCDebug : public EngineSubsystem
{
protected:

    friend class SWorld;
    friend class SRenderWorld;

    WorldSettings m_worldSettings;

    // lifetime owned by SWorld
    std::vector<Chunk*> m_chunks;
};

