// Bradley Christensen - 2022-2025
#include "SRenderEntities.h"
#include "Engine/Renderer/Renderer.h"
#include "Engine/Renderer/VertexUtils.h"
#include "Engine/Renderer/VertexBuffer.h"
#include "Engine/Assets/GridSpriteSheet.h"
#include "Engine/Assets/AssetManager.h"
#include "Engine/Core/ErrorUtils.h"
#include "CAnimation.h"
#include "CCamera.h"
#include "CRender.h"
#include "SCRender.h"



//----------------------------------------------------------------------------------------------------------------------
void SRenderEntities::Startup()
{
    AddWriteDependencies<CRender, Renderer, SCRender>();
    AddReadDependencies<CCamera, CAnimation>();
}



//----------------------------------------------------------------------------------------------------------------------
void SRenderEntities::Run(SystemContext const& context)
{
    auto& renderStorage = g_ecs->GetArrayStorage<CRender>();
    auto& scRender = g_ecs->GetSingleton<SCRender>();
    auto& animStorage = g_ecs->GetMapStorage<CAnimation>();

    g_renderer->SetModelConstants(ModelConstants());

    Camera* activeCamera = nullptr;
    for (auto it = g_ecs->Iterate<CCamera>(context); it.IsValid(); ++it)
    {
        CCamera& camera = *g_ecs->GetComponent<CCamera>(it);
        if (camera.m_isActive)
        {
            activeCamera = &camera.m_camera;
            break;
        }
	}

    if (!activeCamera)
    {
        return;
    }

	AABB2 cameraBounds = activeCamera->GetTranslatedOrthoBounds2D();

    // Clear last frame's verts
    for (auto it : scRender.m_entityVBOsBySpriteSheet)
    {
		VertexBufferID vboID = it.second;
		VertexBuffer* vbo = g_renderer->GetVertexBuffer(vboID);
        if (vbo)
        {
			vbo->ClearVerts();
        }
    }

    // Push back verts for every entity in camera view this frame
    for (auto renderIt = g_ecs->Iterate<CRender, CAnimation>(context); renderIt.IsValid(); ++renderIt)
    {
        CRender& render = *renderStorage.Get(renderIt);
        if (!cameraBounds.IsPointInside(render.m_pos))
        {
            continue;
		}

        CAnimation const& anim = *animStorage.Get(renderIt);

        if (!anim.m_animInstance.IsValid())
        {
            continue;
        }

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

		// Get or create VBO for this sprite sheet
        auto it = scRender.m_entityVBOsBySpriteSheet.find(anim.m_gridSpriteSheet);
        if (it == scRender.m_entityVBOsBySpriteSheet.end())
        {
            VertexBufferID vbo = g_renderer->MakeVertexBuffer();
            scRender.m_entityVBOsBySpriteSheet[anim.m_gridSpriteSheet] = vbo;
        }

        VertexBufferID spriteSheetVBOiD = scRender.m_entityVBOsBySpriteSheet[anim.m_gridSpriteSheet];
        VertexBuffer& spriteSheetVBO = *g_renderer->GetVertexBuffer(spriteSheetVBOiD);
        VertexUtils::AddVertsForAABB2(spriteSheetVBO, spriteAABB, render.m_tint, spriteSheet->GetSpriteUVs(anim.m_animInstance.GetCurrentSpriteIndex()));
    }

	// 1 Draw call per sprite sheet
    for (auto pair : scRender.m_entityVBOsBySpriteSheet)
    {
		AssetID assetID = pair.first;
		VertexBufferID vboID = pair.second;

        GridSpriteSheet* spriteSheet = g_assetManager->Get<GridSpriteSheet>(assetID);
		VertexBuffer* vbo = g_renderer->GetVertexBuffer(vboID);

		ASSERT_OR_DIE(spriteSheet != nullptr, "SRenderEntities::Run - Invalid sprite sheet asset.");
		ASSERT_OR_DIE(vbo != nullptr, "SRenderEntities::Run - Invalid vertex buffer.");

        spriteSheet->SetRendererState();
        g_renderer->DrawVertexBuffer(*vbo);
    }
}



//----------------------------------------------------------------------------------------------------------------------
void SRenderEntities::Shutdown()
{
	ASSERT_OR_DIE(g_assetManager != nullptr, "SRenderEntities::Shutdown - Asset manager is null.");
	ASSERT_OR_DIE(g_renderer != nullptr, "SRenderEntities::Shutdown - Renderer is null.");

    SCRender scRender = g_ecs->GetSingleton<SCRender>();
    for (auto it : scRender.m_entityVBOsBySpriteSheet)
    {
		g_assetManager->Release(it.first);
        g_renderer->ReleaseVertexBuffer(it.second);
	}
}