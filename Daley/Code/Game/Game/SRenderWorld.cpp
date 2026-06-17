// Bradley Christensen - 2022-2026
#include "SRenderWorld.h"
#include "SCAssetManager.h"
#include "SCRender.h"
#include "SCRenderer.h"
#include "SCWorld.h"
#include "WorldShaderCPU.h"
#include "Engine/Assets/GridSpriteSheet.h"
#include "Engine/Assets/ShaderAsset.h"
#include "Engine/Core/EngineCommon.h"
#include "Engine/Core/ErrorUtils.h"
#include "Engine/Renderer/ConstantBuffer.h"



//----------------------------------------------------------------------------------------------------------------------
void SRenderWorld::Startup()
{
    AddWriteDependencies<SCWorld, SCRender, SCAssetManager, SCRenderer>();

    Renderer* renderer = g_ecs->GetSingleton<SCRenderer>().m_renderer;
    AssetManager* assetManager = g_ecs->GetSingleton<SCAssetManager>().m_assetManager;

    SCRender& scRender = g_ecs->GetSingleton<SCRender>();
	scRender.m_staticWorldConstantsBuffer = renderer->MakeConstantBuffer(sizeof(StaticWorldConstants));

    StaticWorldConstants worldConstants;
	worldConstants.m_worldWidth = StaticWorldSettings::s_worldWidth;
	worldConstants.m_numTilesInRow = StaticWorldSettings::s_numTilesInRow;
	worldConstants.m_tileWidth = StaticWorldSettings::s_tileWidth;

	ConstantBuffer* worldConstantsBuffer = renderer->GetConstantBuffer(scRender.m_staticWorldConstantsBuffer);
    worldConstantsBuffer->Update(worldConstants);

	scRender.m_worldShaderAsset = assetManager->AsyncLoad<ShaderAsset>("Data/Shaders/WorldShader.xml");

    // This is mostly to ref count the sheet so it stays loaded
    SCWorld& scWorld = g_ecs->GetSingleton<SCWorld>();
	scWorld.m_worldSpriteSheet = assetManager->AsyncLoad<GridSpriteSheet>("Data/SpriteSheets/Terrain.xml");
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
    Renderer* renderer = g_ecs->GetSingleton<SCRenderer>().m_renderer;
    AssetManager* assetManager = g_ecs->GetSingleton<SCAssetManager>().m_assetManager;

    renderer->ReleaseConstantBuffer(scRender.m_staticWorldConstantsBuffer);

    assetManager->Release(scWorld.m_worldSpriteSheet);
    assetManager->Release(scRender.m_worldShaderAsset);
}



//----------------------------------------------------------------------------------------------------------------------
void SRenderWorld::Run(SystemContext const& context) const
{
	// Write Dependencies
    SCWorld& world = context.GetSingleton<SCWorld>();
	SCRender& scRender = context.GetSingleton<SCRender>();
    Renderer* renderer = context.GetSingleton<SCRenderer>().m_renderer;
    AssetManager* assetManager = context.GetSingleton<SCAssetManager>().m_assetManager;

    world.GenerateVBO(renderer, assetManager);

	GridSpriteSheet const* worldSpriteSheet = assetManager->Get<GridSpriteSheet>(world.m_worldSpriteSheet);
    if (worldSpriteSheet)
    {
        worldSpriteSheet->SetRendererState();
    }
    else
    {
        return;
    }

	ShaderAsset const* shaderAsset = assetManager->Get<ShaderAsset>(scRender.m_worldShaderAsset);
    if (shaderAsset == nullptr)
    {
        return;
    }

    ShaderID worldShader = shaderAsset->GetShaderID();

    renderer->BindTexture(world.m_lightmap, 1);
    renderer->BindShader(worldShader);
	renderer->BindConstantBuffer(scRender.m_staticWorldConstantsBuffer, StaticWorldConstants::GetSlot());
    renderer->BindConstantBuffer(scRender.m_lightingConstantsBuffer, LightingConstants::GetSlot());

	renderer->DrawVertexBuffer(world.m_vbo);
}
