// Bradley Christensen - 2022-2026
#include "SRenderWorld.h"
#include "SCAssetManager.h"
#include "SCRenderer.h"
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
    AddWriteDependencies<SCWorld, SCAssetManager, SCRenderer>();

    SCWorld& scWorld = g_ecs->GetSingleton<SCWorld>();
	SCAssetManager& scAssetManager = g_ecs->GetSingleton<SCAssetManager>();
    SCRenderer& scRenderer = g_ecs->GetSingleton<SCRenderer>();

    AssetManager& assetManager = *scAssetManager.GetAssetManager();
	Renderer& renderer = *scRenderer.GetRenderer();

	scRenderer.m_staticWorldConstantsBuffer = renderer.MakeConstantBuffer(sizeof(StaticWorldConstants));

    StaticWorldConstants worldConstants;
	worldConstants.m_worldWidth = StaticWorldSettings::s_worldWidth;
	worldConstants.m_numTilesInRow = StaticWorldSettings::s_numTilesInRow;
	worldConstants.m_tileWidth = StaticWorldSettings::s_tileWidth;

	ConstantBuffer* worldConstantsBuffer = renderer.GetConstantBuffer(scRenderer.m_staticWorldConstantsBuffer);
    worldConstantsBuffer->Update(worldConstants);

	scRenderer.m_worldShaderAsset = assetManager.AsyncLoad<ShaderAsset>("Data/Shaders/WorldShader.xml");

    // This is mostly to ref count the sheet so it stays loaded
	scWorld.m_worldSpriteSheet = assetManager.AsyncLoad<GridSpriteSheet>("Data/SpriteSheets/Terrain.xml");
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
    SCWorld& scWorld = g_ecs->GetSingleton<SCWorld>();
    SCAssetManager& scAssetManager = g_ecs->GetSingleton<SCAssetManager>();
    SCRenderer& scRenderer = g_ecs->GetSingleton<SCRenderer>();

    AssetManager& assetManager = *scAssetManager.GetAssetManager();
    Renderer& renderer = *scRenderer.GetRenderer();

    renderer.ReleaseConstantBuffer(scRenderer.m_staticWorldConstantsBuffer);

    assetManager.Release(scWorld.m_worldSpriteSheet);
    assetManager.Release(scRenderer.m_worldShaderAsset);
}



//----------------------------------------------------------------------------------------------------------------------
void SRenderWorld::Run(SystemContext const& context) const
{
	// Write Dependencies
    SCWorld& scWorld = context.GetSingleton<SCWorld>();
    SCAssetManager& scAssetManager = context.GetSingleton<SCAssetManager>();
    SCRenderer& scRenderer = context.GetSingleton<SCRenderer>();

    AssetManager& assetManager = *scAssetManager.GetAssetManager();
    Renderer& renderer = *scRenderer.GetRenderer();

    scWorld.GenerateVBO(renderer, assetManager);

	GridSpriteSheet const* worldSpriteSheet = assetManager.Get<GridSpriteSheet>(scWorld.m_worldSpriteSheet);
    if (worldSpriteSheet)
    {
        worldSpriteSheet->SetRendererState();
    }
    else
    {
        return;
    }

	ShaderAsset const* shaderAsset = assetManager.Get<ShaderAsset>(scRenderer.m_worldShaderAsset);
    if (shaderAsset == nullptr)
    {
        return;
    }

    ShaderID worldShader = shaderAsset->GetShaderID();

    renderer.BindTexture(scWorld.m_lightmap, 1);
    renderer.BindShader(worldShader);
	renderer.BindConstantBuffer(scRenderer.m_staticWorldConstantsBuffer, StaticWorldConstants::GetSlot());
    renderer.BindConstantBuffer(scRenderer.m_lightingConstantsBuffer, LightingConstants::GetSlot());

	renderer.DrawVertexBuffer(scWorld.m_vbo);
}
