// Bradley Christensen - 2022-2025
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
    AddWriteDependencies<SCRender, Renderer, AssetManager>();
    AddReadDependencies<SCWorld>();

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
}



//----------------------------------------------------------------------------------------------------------------------
void SRenderWorld::Run(SystemContext const&)
{
	SCRender& scRender = g_ecs->GetSingleton<SCRender>();
    SCWorld const& world = g_ecs->GetSingleton<SCWorld>();

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
    if (shaderAsset)
    {
		scRender.m_worldShaderID = shaderAsset->GetShaderID();
    }
    else
    {
		scRender.m_worldShaderID = RendererUtils::InvalidID;
        return;
    }

    g_renderer->BindShader(scRender.m_worldShaderID);
	g_renderer->BindConstantBuffer(scRender.m_staticWorldConstantsBuffer, StaticWorldConstants::GetSlot());

    // Todo: Generate world vbo
	//g_renderer->DrawVertexBuffer(world.m_vbo);
}



//----------------------------------------------------------------------------------------------------------------------
void SRenderWorld::Shutdown()
{
    auto& scWorld = g_ecs->GetSingleton<SCWorld>();
	g_assetManager->Release(scWorld.m_worldSpriteSheet);

    SCRender& scRender = g_ecs->GetSingleton<SCRender>();
	g_assetManager->Release(scRender.m_worldShaderAsset);
	g_renderer->ReleaseConstantBuffer(scRender.m_spriteSheetConstantsBuffer);
}
