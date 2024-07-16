// Bradley Christensen - 2024
#pragma once
#include "Engine/Core/EngineSubsystem.h"
#include "Engine/Math/Grid2D.h"
#include "TileDef.h"



class VertexBuffer;



//----------------------------------------------------------------------------------------------------------------------
class SCWorld : public EngineSubsystem
{
    friend class SWorld;
    friend class SRenderWorld;

protected:

    // Data (SWorld)
    Vec2 m_offset = Vec2::ZeroVector;
    float m_tileSize = 1.f;
	Grid2D<TileID> m_tileIds;

    // Rendering (lifetime owned by SRenderWorld)
    VertexBuffer* m_tileVbo = nullptr;
};

