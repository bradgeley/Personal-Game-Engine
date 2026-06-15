// Bradley Christensen - 2022-2026
#include "SRenderWorld.h"
#include "SCRender.h"
#include "SCWorld.h"
#include "WorldShaderCPU.h"
#include "Engine/Assets/AssetManager.h"
#include "Engine/Assets/GridSpriteSheet.h"
#include "Engine/Assets/ShaderAsset.h"
#include "Engine/Core/EngineCommon.h"
#include "Engine/Core/ErrorUtils.h"
#include "Engine/Renderer/ConstantBuffer.h"
#include "Engine/Renderer/Renderer.h"



//----------------------------------------------------------------------------------------------------------------------
void SRenderWorld::Startup()
{
    AddWriteDependencies<SCWorld, SCRender, AssetManager, Renderer>();

    SCRender& scRender = g_ecs->GetSingleton<SCRender>();
	scRender.m_staticWorldConstantsBuffer = g_renderer->MakeConstantBuffer(sizeof(StaticWorldConstants));

    StaticWorldConstants worldConstants;
	worldConstants.m_worldWidth = StaticWorldSettings::s_worldWidth;
	worldConstants.m_numTilesInRow = StaticWorldSettings::s_numTilesInRow;
	worldConstants.m_tileWidth = StaticWorldSettings::s_tileWidth;

	ConstantBuffer* worldConstantsBuffer = g_renderer->GetConstantBuffer(scRender.m_staticWorldConstantsBuffer);
    worldConstantsBuffer->Update(worldConstants);

	scRender.m_worldShaderAsset = g_assetManager->AsyncLoad<ShaderAsset>("Data/Shaders/WorldShader.xml");

    // This is mostly to ref count the sheet so it stays loaded
    SCWorld& scWorld = g_ecs->GetSingleton<SCWorld>();
	scWorld.m_worldSpriteSheet = g_assetManager->AsyncLoad<GridSpriteSheet>("Data/SpriteSheets/Terrain.xml");
	ASSERT_OR_DIE(scWorld.m_worldSpriteSheet != AssetID::Invalid, "Failed to load world sprite sheet in SRenderWorld::Startup");

    scWorld.ForEachVisibleTile([&scWorld](IntVec2 const& tileCoords, int)
    {
        Tile& tile = scWorld.m_tiles.GetRef(tileCoords);
        tile.SetVertsDirty(true);
        return true;
    });
}



//----------------------------------------------------------------------------------------------------------------------
void SRenderWorld::Shutdown() const
{
    auto& scWorld = g_ecs->GetSingleton<SCWorld>();
    SCRender& scRender = g_ecs->GetSingleton<SCRender>();

    g_renderer->ReleaseConstantBuffer(scRender.m_staticWorldConstantsBuffer);

    g_assetManager->Release(scWorld.m_worldSpriteSheet);
    g_assetManager->Release(scRender.m_worldShaderAsset);
}



//----------------------------------------------------------------------------------------------------------------------
void SRenderWorld::Run(SystemContext const& context) const
{
	// Write Dependencies
    SCWorld& world = context.GetSingleton<SCWorld>();
	SCRender& scRender = context.GetSingleton<SCRender>();
	// g_assetManager
    // g_renderer

    world.GenerateVBO(g_renderer, g_assetManager);

	GridSpriteSheet* worldSpriteSheet = g_assetManager->Get<GridSpriteSheet>(world.m_worldSpriteSheet);
    if (worldSpriteSheet)
    {
        worldSpriteSheet->SetRendererState();
    }
    else
    {
        return;
    }

	ShaderAsset* shaderAsset = g_assetManager->Get<ShaderAsset>(scRender.m_worldShaderAsset);
    if (shaderAsset == nullptr)
    {
        return;
    }

    ShaderID worldShader = shaderAsset->GetShaderID();

    g_renderer->BindTexture(world.m_lightmap, 1);
    g_renderer->BindShader(worldShader);
	g_renderer->BindConstantBuffer(scRender.m_staticWorldConstantsBuffer, StaticWorldConstants::GetSlot());
    g_renderer->BindConstantBuffer(scRender.m_lightingConstantsBuffer, LightingConstants::GetSlot());

	g_renderer->DrawVertexBuffer(world.m_vbo);
}
