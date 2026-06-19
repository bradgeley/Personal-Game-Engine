// Bradley Christensen - 2022-2026
#include "SRenderCollisionEffects.h"
#include "CCollision.h"
#include "CCollisionEffect.h"
#include "CTransform.h"
#include "SCRenderer.h"
#include "Engine/Renderer/Renderer.h"
#include "Engine/Renderer/Vertex_PCU.h"
#include "Engine/Renderer/VertexBuffer.h"
#include "Engine/Renderer/VertexUtils.h"



//----------------------------------------------------------------------------------------------------------------------
void SRenderCollisionEffects::Startup()
{
    AddWriteDependencies<SCRenderer>();
	AddReadDependencies<CCollisionEffect, CCollision, CTransform>();
}



//----------------------------------------------------------------------------------------------------------------------
void SRenderCollisionEffects::Shutdown() const
{
    SCRenderer& scRenderer = g_ecs->GetSingleton<SCRenderer>();

    if (scRenderer.m_immediateVBO != RendererUtils::InvalidID)
    {
	    scRenderer.GetRenderer()->ReleaseVertexBuffer(scRenderer.m_immediateVBO);
    }
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

    if (scRenderer.m_immediateVBO == RendererUtils::InvalidID)
    {
        scRenderer.m_immediateVBO = renderer.MakeVertexBuffer<Vertex_PCU>();
    }

    VertexBuffer* vbo = renderer.GetVertexBuffer(scRenderer.m_immediateVBO);
    vbo->ClearVerts();

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

        VertexUtils::AddVertsForDisc2D(*vbo, collisionPos, collision.m_radius, 32, tint, AABB2::ZeroToOne, collisionEffect.m_depth);
	}

    renderer.BindShader();
    renderer.BindTexture();
    renderer.DrawVertexBuffer(scRenderer.m_immediateVBO);
}
