// Bradley Christensen - 2022-2026
#include "SRenderCollisionEffects.h"
#include "CCollision.h"
#include "CCollisionEffect.h"
#include "DiscShaderCPU.h"
#include "CTransform.h"
#include "SCRenderer.h"
#include "Engine/Renderer/Renderer.h"
#include "Engine/Renderer/InstanceBuffer.h"



//----------------------------------------------------------------------------------------------------------------------
void SRenderCollisionEffects::Startup()
{
    AddWriteDependencies<SCRenderer>();
	AddReadDependencies<CCollisionEffect, CCollision, CTransform>();
}



//----------------------------------------------------------------------------------------------------------------------
void SRenderCollisionEffects::Shutdown() const
{

}



//----------------------------------------------------------------------------------------------------------------------
void SRenderCollisionEffects::Run(SystemContext const& context) const
{
	// Write Dependencies
    SCRenderer& scRenderer = context.GetSingleton<SCRenderer>();
	Renderer& renderer = *scRenderer.GetRenderer();

	// Read Dependencies
	auto& transStorage = context.GetArrayStorageConst<CTransform>();
	auto& collisionStorage = context.GetArrayStorageConst<CCollision>();
	auto& collisionEffectStorage = context.GetArrayStorageConst<CCollisionEffect>();

	InstanceBuffer& discIBO = *renderer.GetInstanceBuffer(scRenderer.m_discInstanceBuffer);

	for (auto it = context.Iterate<CCollisionEffect, CCollision, CTransform>(); it.IsValid(); ++it)
	{
		CCollisionEffect const& collisionEffect = collisionEffectStorage[it];
        if (!collisionEffect.m_shouldRender)
        {
            continue;
		}

		CCollision const& collision = collisionStorage[it];
		CTransform const& transform = transStorage[it];

        Rgba8 tint = collisionEffect.m_tint;
        Vec2 collisionPos = transform.m_pos + collision.m_offset;

		DiscRenderInstance discInstance;
		discInstance.m_position = Vec3(collisionPos, collisionEffect.m_depth);
		discInstance.m_radius = collision.m_radius;
		discInstance.m_tint = collisionEffect.m_tint;
		discIBO.AddInstance(discInstance);
	}
}
