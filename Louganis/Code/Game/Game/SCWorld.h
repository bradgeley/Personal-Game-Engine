// Bradley Christensen - 2024
#pragma once
#include "Engine/Core/EngineSubsystem.h"
#include "Engine/Math/Grid2D.h"
#include "TileDef.h"



//----------------------------------------------------------------------------------------------------------------------
class SCWorld : public EngineSubsystem
{
public:

    void Startup() override;
    void Update(float deltaSeconds) override;
    void Render() const override;
    void Shutdown() override;

protected:

	Grid2D<TileID> m_tileIds;
};

