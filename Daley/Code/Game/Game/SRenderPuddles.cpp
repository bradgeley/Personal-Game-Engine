// Bradley Christensen - 2022-2026
#include "SRenderPuddles.h"
#include "CCollision.h"
#include "CPuddle.h"
#include "DiscShaderCPU.h"
#include "CTransform.h"
#include "SCRenderer.h"
#include "Engine/ECS/SystemContext.h"
#include "Engine/Renderer/Renderer.h"
#include "Engine/Renderer/InstanceBuffer.h"



//----------------------------------------------------------------------------------------------------------------------
void SRenderPuddles::Startup()
{
    AddWriteDependencies<SCRenderer>();
	AddReadDependencies<CPuddle, CCollision, CTransform>();
}



//----------------------------------------------------------------------------------------------------------------------
void SRenderPuddles::Shutdown() const
{

}



//----------------------------------------------------------------------------------------------------------------------
void SRenderPuddles::Run(SystemContext const& context) const
{
	// Write Dependencies
    SCRenderer& scRenderer = context.GetSingleton<SCRenderer>();
	Renderer& renderer = *scRenderer.GetRenderer();

	// Read Dependencies
	auto& transStorage = context.GetArrayStorageConst<CTransform>();
	auto& collisionStorage = context.GetArrayStorageConst<CCollision>();
	auto& puddleStorage = context.GetArrayStorageConst<CPuddle>();

	InstanceBuffer& discIBO = *renderer.GetInstanceBuffer(scRenderer.m_discInstanceBuffer);

	for (auto it = context.Iterate<CPuddle, CCollision, CTransform>(); it.IsValid(); ++it)
	{
		CPuddle const& puddle = puddleStorage[it];
		if (puddle.m_tint.a == 0)
		{
			continue;
		}

		CCollision const& collision = collisionStorage[it];
		CTransform const& transform = transStorage[it];

        Rgba8 tint = puddle.m_tint;
        Vec2 collisionPos = transform.m_pos;

		DiscRenderInstance discInstance;
		discInstance.m_position = Vec3(collisionPos, puddle.m_depth);
		discInstance.m_radius = collision.m_radius;
		discInstance.m_tint = puddle.m_tint;
		discIBO.AddInstance(discInstance);
	}
}
