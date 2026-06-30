// Bradley Christensen - 2022-2026
#include "SRenderDiscs.h"
#include "SCAssetManager.h"
#include "DiscShaderCPU.h"
#include "SCRenderer.h"
#include "Engine/Assets/AssetManager.h"
#include "Engine/Assets/ShaderAsset.h"
#include "Engine/Renderer/Renderer.h"
#include "Engine/Renderer/ConstantBuffer.h"
#include "Engine/Renderer/InstanceBuffer.h"



constexpr int NUM_SIDES = 32; // Supports powers of 2 from 4 to 64
static_assert(NUM_SIDES >= 4 && NUM_SIDES <= 64 && (NUM_SIDES & (NUM_SIDES - 1)) == 0, "NUM_SIDES must be a power of 2 between 4 and 64");



//----------------------------------------------------------------------------------------------------------------------
void SRenderDiscs::Startup()
{
    AddWriteDependencies<SCRenderer>();
    AddReadDependencies<SCAssetManager>();

    SCRenderer& scRenderer = g_ecs->GetSingleton<SCRenderer>();
    Renderer& renderer = *scRenderer.GetRenderer();

    scRenderer.m_discConstantsBuffer = renderer.MakeConstantBuffer(sizeof(DiscRenderConstants));
    scRenderer.m_discInstanceBuffer = renderer.MakeInstanceBuffer<DiscRenderInstance>();

	SCAssetManager& scAssetManager = g_ecs->GetSingleton<SCAssetManager>();
	AssetManager& assetManager = *scAssetManager.GetAssetManager();

	scRenderer.m_discShaderAsset = assetManager.AsyncLoad<ShaderAsset>("Data/Shaders/DiscShader.xml");
}



//----------------------------------------------------------------------------------------------------------------------
void SRenderDiscs::Shutdown() const
{
    SCRenderer& scRenderer = g_ecs->GetSingleton<SCRenderer>();
    Renderer& renderer = *scRenderer.GetRenderer();

	renderer.ReleaseConstantBuffer(scRenderer.m_discConstantsBuffer);
	renderer.ReleaseInstanceBuffer(scRenderer.m_discInstanceBuffer);

    SCAssetManager& scAssetManager = g_ecs->GetSingleton<SCAssetManager>();
    AssetManager& assetManager = *scAssetManager.GetAssetManager();

	assetManager.Release(scRenderer.m_discShaderAsset);
}



//----------------------------------------------------------------------------------------------------------------------
void SRenderDiscs::Run(SystemContext const& context) const
{
	// Write Dependencies
    SCRenderer& scRenderer = context.GetSingleton<SCRenderer>();
    Renderer& renderer = *scRenderer.GetRenderer();

	// Read Dependencies
	SCAssetManager const& scAssetManager = context.GetSingletonConst<SCAssetManager>();
    AssetManager const& assetManager = *scAssetManager.GetAssetManager();

	ShaderAsset const* discShaderAsset = assetManager.Get<ShaderAsset>(scRenderer.m_discShaderAsset);
    if (!discShaderAsset)
    {
        InstanceBuffer& discIBO = *renderer.GetInstanceBuffer(scRenderer.m_discInstanceBuffer);
        discIBO.ClearInstances();
        return;
	}

	ConstantBuffer& cbo = *renderer.GetConstantBuffer(scRenderer.m_discConstantsBuffer);
	InstanceBuffer& discIBO = *renderer.GetInstanceBuffer(scRenderer.m_discInstanceBuffer);

	DiscRenderConstants constants;
    constants.m_numSides = NUM_SIDES;
	cbo.Update(constants);

    renderer.BindConstantBuffer(scRenderer.m_discConstantsBuffer, DiscRenderConstants::GetSlot());
    renderer.BindShader(discShaderAsset->GetShaderID());
    renderer.BindTexture();
    renderer.DrawInstanced(constants.m_numSides * 3, discIBO);

	discIBO.ClearInstances();
}
