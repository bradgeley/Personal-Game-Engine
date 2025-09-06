// Bradley Christensen - 2022-2025
#include "SRenderEntities.h"
#include "Engine/Renderer/Renderer.h"
#include "Engine/Renderer/VertexUtils.h"
#include "Engine/Renderer/VertexBuffer.h"
#include "Engine/Assets/GridSpriteSheet.h"
#include "Engine/Assets/AssetManager.h"
#include "CAnimation.h"
#include "CCamera.h"
#include "CRender.h"



//----------------------------------------------------------------------------------------------------------------------
void SRenderEntities::Startup()
{
    AddWriteDependencies<CRender, Renderer>();
    AddReadDependencies<CCamera, CAnimation>();
}



//----------------------------------------------------------------------------------------------------------------------
void SRenderEntities::Run(SystemContext const& context)
{
    auto& renderStorage = g_ecs->GetArrayStorage<CRender>();
    auto& animStorage = g_ecs->GetMapStorage<CAnimation>();

    g_renderer->SetModelConstants(ModelConstants());

    for (auto renderIt = g_ecs->Iterate<CRender, CAnimation>(context); renderIt.IsValid(); ++renderIt)
    {
        CRender& render = *renderStorage.Get(renderIt);
        CAnimation const& anim = *animStorage.Get(renderIt);

        if (render.m_vbo == RendererUtils::InvalidID)
        {
            render.m_vbo = g_renderer->MakeVertexBuffer();
        }

        VertexBuffer& vbo = *g_renderer->GetVertexBuffer(render.m_vbo);
        vbo.ClearVerts();

        GridSpriteSheet* spriteSheet = g_assetManager->Get<GridSpriteSheet>(anim.m_gridSpriteSheet);
        if (!spriteSheet)
        {
            continue;
        }

        AABB2 spriteAABB;
        float spriteAspect = spriteSheet->GetSpriteAspect();
        if (spriteAspect <= 1.f)
        {
            spriteAABB.mins = render.m_pos - Vec2(render.m_scale, render.m_scale);
            spriteAABB.maxs = spriteAABB.mins + Vec2(2.f * render.m_scale, 2.f * render.m_scale / spriteAspect);
        }
        else
        {
            spriteAABB.mins = render.m_pos - Vec2(render.m_scale * spriteAspect, render.m_scale);
            spriteAABB.maxs = spriteAABB.mins + Vec2(2.f * render.m_scale * spriteAspect, 2.f * render.m_scale);
        }

        VertexUtils::AddVertsForAABB2(vbo, spriteAABB, render.m_tint, spriteSheet->GetSpriteUVs(anim.m_animInstance.GetCurrentSpriteIndex()));

        spriteSheet->SetRendererState();
        g_renderer->DrawVertexBuffer(render.m_vbo);
    }
}



//----------------------------------------------------------------------------------------------------------------------
void SRenderEntities::Shutdown()
{

}