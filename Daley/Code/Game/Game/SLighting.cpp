// Bradley Christensen - 2022-2026
#include "SLighting.h"
#include "SCRender.h"
#include "SCWorld.h"
#include "SCCamera.h"
#include "SCLighting.h"
#include "SCRenderer.h"
#include "TileDef.h"
#include "WorldSettings.h"
#include "WorldShaderCPU.h"
#include "Engine/Debug/DevConsoleUtils.h"
#include "Engine/Math/MathUtils.h"
#include "Engine/Renderer/ConstantBuffer.h"



//----------------------------------------------------------------------------------------------------------------------
void SLighting::Startup()
{
    AddReadDependencies<SCCamera, SCRender>();
	AddWriteDependencies<SCWorld, SCLighting, SCRenderer>();

    SCRender& scRender = g_ecs->GetSingleton<SCRender>();
    scRender.m_lightingConstantsBuffer = g_renderer->MakeConstantBuffer(sizeof(LightingConstants));

	DevConsoleUtils::AddDevConsoleCommand("ToggleLighting", &SLighting::ToggleLighting);

	SCWorld& scWorld = g_ecs->GetSingleton<SCWorld>();
    scWorld.ForEachVisibleTile([&scWorld](IntVec2 const& tileCoords, int)
    {
        Tile& tile = scWorld.m_tiles.GetRef(tileCoords);
        tile.SetLightingDirty(true);
        return true;
	});
}



//----------------------------------------------------------------------------------------------------------------------
void SLighting::Shutdown() const
{
    SCRender& scRender = g_ecs->GetSingleton<SCRender>();
    g_renderer->ReleaseConstantBuffer(scRender.m_lightingConstantsBuffer);

    DevConsoleUtils::RemoveDevConsoleCommand("ToggleLighting", &SLighting::ToggleLighting);
}



//----------------------------------------------------------------------------------------------------------------------
void SLighting::Run(SystemContext const& context) const
{
	// Read Dependencies
    SCRender const& scRender = context.GetSingletonConst<SCRender>();
    SCCamera const& scCamera = context.GetSingletonConst<SCCamera>();

    // Write Dependencies
    SCWorld& scWorld = context.GetSingleton<SCWorld>();
    SCLighting& scLighting = context.GetSingleton<SCLighting>();
	Renderer& renderer = *context.GetSingleton<SCRenderer>().m_renderer;

    if (!scWorld.m_isLightingDirty)
    {
        return;
	}

	scWorld.m_isLightingDirty = false;

    static IntVec2 neighborOffsets[4] = { IntVec2(1, 0), IntVec2(-1, 0), IntVec2(0, 1), IntVec2(0, -1) };

	LightingConstants lightingConstants;
    Rgba8::White.GetAsFloats(lightingConstants.m_ambientLightTint);
    Rgba8::LightGray.GetAsFloats(lightingConstants.m_outdoorLightTint);
	Rgba8::LightOrange.GetAsFloats(lightingConstants.m_indoorLightTint);
    lightingConstants.m_ambientLightIntensity = 0.01f; // 0 = pitch black, 1 = full brightness
	lightingConstants.m_isLightingEnabled = scLighting.m_isLightingEnabled ? 1 : 0;

    ConstantBuffer* lightingCbo = renderer.GetConstantBuffer(scRender.m_lightingConstantsBuffer);
    lightingCbo->Update(lightingConstants);

	AABB2 cameraBounds = scCamera.m_camera.GetTranslatedOrthoBounds2D();

    for (int y = StaticWorldSettings::s_visibleWorldBeginIndexY; y <= StaticWorldSettings::s_visibleWorldEndIndexY; ++y)
    {
        for (int x = StaticWorldSettings::s_visibleWorldBeginIndexX; x <= StaticWorldSettings::s_visibleWorldEndIndexX; ++x)
        {
			Tile const& tile = scWorld.m_tiles.GetRef(IntVec2(x, y));
            if (tile.IsLightingDirty())
            {
                scLighting.m_dirtyLightingTiles.insert(IntVec2(x, y));
			}
        }
    }

    for (auto it = scLighting.m_dirtyLightingTiles.begin(); it != scLighting.m_dirtyLightingTiles.end(); )
    {
        IntVec2 tileCoords = *it;
        it = scLighting.m_dirtyLightingTiles.erase(it);

        Tile& tile = scWorld.m_tiles.GetRef(tileCoords);
        tile.SetLightingDirty(false);
        TileDef const& tileDef = *TileDef::GetTileDef(tile.m_id);

        uint8_t currentIndoorLighting = tile.GetIndoorLighting();
        uint8_t currentOutdoorLighting = tile.GetOutdoorLighting();

        uint8_t innateOutdoorLight = tile.IsOpaque() ? 0 : StaticWorldSettings::s_maxOutdoorLighting; // All tiles that are not Opaque get full outdoor light (for now)
        uint8_t innateIndoorLight = tileDef.m_indoorLight;

        uint8_t correctOutdoorLighting = innateOutdoorLight; // All tiles that are not Opaque get full outdoor light (for now)
        uint8_t correctIndoorLighting = innateIndoorLight;

        bool lightingChanged = false;
        for (IntVec2 const& neighborOffset : neighborOffsets)
        {
            IntVec2 neighborCoords = tileCoords + neighborOffset;
            if (!scWorld.IsTileVisible(neighborCoords))
            {
                continue;
            }
            Tile& neighborTile = scWorld.m_tiles.GetRef(neighborCoords);
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
            tile.SetVertsDirty(true);
            scWorld.m_isVBODirty = true;

            for (auto& neighborOffset : neighborOffsets)
            {
                IntVec2 neighborCoords = tileCoords + neighborOffset;
                if (!scWorld.IsTileVisible(neighborCoords))
                {
                    continue;
                }
                Tile& neighborTile = scWorld.m_tiles.GetRef(neighborCoords);
                neighborTile.SetLightingDirty(true);
                scLighting.m_dirtyLightingTiles.insert(neighborCoords);
            }
        }
        it = scLighting.m_dirtyLightingTiles.begin();
    }

    scWorld.GenerateLightmap(&renderer);
}



//----------------------------------------------------------------------------------------------------------------------
bool SLighting::ToggleLighting(NamedProperties&)
{
	SCLighting& scLighting = g_ecs->GetSingleton<SCLighting>();
	scLighting.m_isLightingEnabled = !scLighting.m_isLightingEnabled;

    SCWorld& scWorld = g_ecs->GetSingleton<SCWorld>();
    scWorld.m_isLightingDirty = true;

    return false;
}
