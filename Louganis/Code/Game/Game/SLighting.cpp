// Bradley Christensen - 2022-2025
#include "SLighting.h"
#include "Chunk.h"
#include "SCRender.h"
#include "SCTime.h"
#include "SCWorld.h"
#include "SCCamera.h"
#include "SCLighting.h"
#include "TileDef.h"
#include "WorldShaderCPU.h"
#include "Engine/Debug/DevConsoleUtils.h"
#include "Engine/Math/MathUtils.h"
#include "Engine/Renderer/Renderer.h"
#include "Engine/Renderer/ConstantBuffer.h"



//----------------------------------------------------------------------------------------------------------------------
void SLighting::Startup()
{
    AddReadDependencies<SCTime, SCWorld, SCCamera, SCRender>();
	AddWriteDependencies<SCLighting, Renderer>();

    SCRender& scRender = g_ecs->GetSingleton<SCRender>();
    scRender.m_lightingConstantsBuffer = g_renderer->MakeConstantBuffer(sizeof(LightingConstants));

	DevConsoleUtils::AddDevConsoleCommand("ToggleLighting", &SLighting::ToggleLighting);
}



//----------------------------------------------------------------------------------------------------------------------
void SLighting::Run(SystemContext const&)
{
    SCRender const& scRender = g_ecs->GetSingleton<SCRender>();
    SCTime const& scTime = g_ecs->GetSingleton<SCTime>();
	SCWorld const& scWorld = g_ecs->GetSingleton<SCWorld>();
	SCCamera const& scCamera = g_ecs->GetSingleton<SCCamera>();
	SCLighting& scLighting = g_ecs->GetSingleton<SCLighting>();

    static IntVec2 neighborOffsets[4] = { IntVec2(1, 0), IntVec2(-1, 0), IntVec2(0, 1), IntVec2(0, -1) };

	Rgba8 timeOfDayTint = scWorld.m_worldSettings.m_timeOfDayTints[(size_t) scTime.m_timeOfDay];
    TimeOfDay nextTimeOfDay = (TimeOfDay) MathUtils::IncrementIntInRange((int) scTime.m_timeOfDay, 0, (int) TimeOfDay::Count - 1, true);
	Rgba8 nextTimeOfDayTint = scWorld.m_worldSettings.m_timeOfDayTints[(size_t) nextTimeOfDay];
	Rgba8 outdoorLightTint = Rgba8::Lerp(timeOfDayTint, nextTimeOfDayTint, scTime.m_isTransitioning ? (float) scTime.m_dayTimer.GetElapsedFraction() : 0.f);

	LightingConstants lightingConstants;
    Rgba8::White.GetAsFloats(lightingConstants.m_ambientLightTint);
	outdoorLightTint.GetAsFloats(lightingConstants.m_outdoorLightTint);
	Rgba8::LightOrange.GetAsFloats(lightingConstants.m_indoorLightTint);
    lightingConstants.m_ambientLightIntensity = 0.01f; // 0 = pitch black, 1 = full brightness
	lightingConstants.m_isLightingEnabled = scLighting.m_isLightingEnabled ? 1 : 0;

    ConstantBuffer* lightingCbo = g_renderer->GetConstantBuffer(scRender.m_lightingConstantsBuffer);
    lightingCbo->Update(lightingConstants);

	AABB2 cameraBounds = scCamera.m_camera.GetTranslatedOrthoBounds2D();
    scWorld.ForEachChunkOverlappingAABB(cameraBounds, [&](Chunk& chunk)
    {
        if (!chunk.m_isLightingDirty)
        {
            return true;
        }
        for (int tileIndex = 0; tileIndex < chunk.m_tiles.Size(); ++tileIndex)
        {
            Tile& tile = chunk.m_tiles.GetRef(tileIndex);
            if (tile.IsLightingDirty())
            {
				scLighting.m_dirtyLightingTiles.insert(WorldCoords(chunk.m_chunkCoords, chunk.m_tiles.GetCoordsForIndex(tileIndex)));
            }
		}
        return true; // keep iterating
	});

	for (auto it = scLighting.m_dirtyLightingTiles.begin(); it != scLighting.m_dirtyLightingTiles.end();)
    {
        WorldCoords worldCoords = *it;
        it = scLighting.m_dirtyLightingTiles.erase(it);

        Chunk* chunk = scWorld.GetActiveChunk(worldCoords);
        if (!chunk)
        {
            continue;
        }

		Tile& tile = chunk->m_tiles.GetRef(worldCoords.m_localTileCoords);
        tile.SetLightingDirty(false);
		TileDef const& tileDef = *TileDef::GetTileDef(tile.m_id);

		uint8_t currentIndoorLighting = tile.GetIndoorLighting();
		uint8_t currentOutdoorLighting = tile.GetOutdoorLighting();

		uint8_t innateOutdoorLight = tile.IsOpaque() ? 0 : StaticWorldSettings::s_maxOutdoorLighting; // All tiles that are not Opaque get full outdoor light (for now)
		uint8_t innateIndoorLight = tileDef.m_indoorLight;

		uint8_t correctOutdoorLighting = innateOutdoorLight; // All tiles that are not Opaque get full outdoor light (for now)
        uint8_t correctIndoorLighting = innateIndoorLight;

        bool lightingChanged = false;
        for (auto& neighborOffset : neighborOffsets)
        {
			WorldCoords neighborCoords = scWorld.GetWorldCoordsAtOffset(worldCoords, neighborOffset);
            Chunk* neighborChunk = scWorld.GetActiveChunk(neighborCoords);
            if (!neighborChunk)
            {
                continue;
			}
			Tile& neighborTile = neighborChunk->m_tiles.GetRef(neighborCoords.m_localTileCoords);
			uint8_t neighborOutdoorLight = neighborTile.GetOutdoorLighting();
            if (neighborOutdoorLight > correctOutdoorLighting + 1)
            {
                correctOutdoorLighting = neighborOutdoorLight - 1;
			}
			uint8_t neighborIndoorLight = neighborTile.GetIndoorLighting();
            if (neighborIndoorLight > correctIndoorLighting + 1)
            {
                correctIndoorLighting = neighborIndoorLight - 1;
            }
        }

        if (correctIndoorLighting != currentIndoorLighting)
        {
			tile.SetIndoorLighting(correctIndoorLighting);
            lightingChanged = true;
        }

        if (correctOutdoorLighting != currentOutdoorLighting)
		{
            tile.SetOutdoorLighting(correctOutdoorLighting);
            lightingChanged = true;
		}

        if (lightingChanged)
        {
			scLighting.m_dirtyChunks.insert(chunk);

            for (auto& neighborOffset : neighborOffsets)
            {
                WorldCoords neighborCoords = scWorld.GetWorldCoordsAtOffset(worldCoords, neighborOffset);
                Chunk* neighborChunk = scWorld.GetActiveChunk(neighborCoords);
                if (!neighborChunk)
                {
                    continue;
                }
                Tile& neighborTile = neighborChunk->m_tiles.GetRef(neighborCoords.m_localTileCoords);
                neighborTile.SetLightingDirty(true);
                neighborChunk->m_isLightingDirty = true;
				scLighting.m_dirtyLightingTiles.insert(neighborCoords);
            }
        }
        it = scLighting.m_dirtyLightingTiles.begin();
    }

    for (Chunk* dirtyChunk : scLighting.m_dirtyChunks)
    {
        dirtyChunk->GenerateLightmap();
	}
    scLighting.m_dirtyChunks.clear();
}



//----------------------------------------------------------------------------------------------------------------------
void SLighting::Shutdown()
{
    SCRender& scRender = g_ecs->GetSingleton<SCRender>();
    g_renderer->ReleaseConstantBuffer(scRender.m_lightingConstantsBuffer);

	DevConsoleUtils::RemoveDevConsoleCommand("ToggleLighting", &SLighting::ToggleLighting);
}



//----------------------------------------------------------------------------------------------------------------------
bool SLighting::ToggleLighting(NamedProperties&)
{
	SCLighting& scLighting = g_ecs->GetSingleton<SCLighting>();
	scLighting.m_isLightingEnabled = !scLighting.m_isLightingEnabled;
    return false;
}
