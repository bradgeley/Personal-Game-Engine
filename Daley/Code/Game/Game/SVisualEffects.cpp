// Bradley Christensen - 2022-2026
#include "SVisualEffects.h"
#include "CAnimation.h"
#include "CHealth.h"
#include "CRender.h"
#include "CTime.h"
#include "SCAssetManager.h"
#include "SCRenderer.h"
#include "SpriteShaderCPU.h"
#include "Engine/Assets/AssetManager.h"
#include "Engine/Assets/GridSpriteSheet.h"
#include "Engine/ECS/AdminSystem.h"
#include "Engine/ECS/SystemContext.h"
#include "Engine/Renderer/InstanceBuffer.h"
#include "Engine/Renderer/Renderer.h"



//----------------------------------------------------------------------------------------------------------------------
void SVisualEffects::Startup()
{
	AddReadDependencies<CHealth, CTime, SCAssetManager>();
	AddWriteDependencies<CAnimation, CRender, SCRenderer>();

	SCAssetManager& scAssetManager = g_ecs->GetSingleton<SCAssetManager>();
	AssetManager& assetManager = *scAssetManager.GetAssetManager();

	SCRenderer& scRenderer = g_ecs->GetSingleton<SCRenderer>();
	Renderer& renderer = *scRenderer.GetRenderer();
	scRenderer.m_statusEffectsSpriteSheet = assetManager.AsyncLoad<GridSpriteSheet>("Data/SpriteSheets/StatusEffects.xml");
	scRenderer.m_instancesPerSpriteSheet[scRenderer.m_statusEffectsSpriteSheet] = renderer.MakeInstanceBuffer<SpriteInstance>();
}



//----------------------------------------------------------------------------------------------------------------------
void SVisualEffects::Shutdown() const
{
    SCAssetManager& scAssetManager = g_ecs->GetSingleton<SCAssetManager>();
    AssetManager& assetManager = *scAssetManager.GetAssetManager();

    SCRenderer& scRenderer = g_ecs->GetSingleton<SCRenderer>();
	assetManager.Release(scRenderer.m_statusEffectsSpriteSheet);
}



//----------------------------------------------------------------------------------------------------------------------
void SVisualEffects::Run(SystemContext const& context) const
{
    // Read Dependencies
    auto const& healthStorage = context.GetArrayStorageConst<CHealth>();
    auto const& timeStorage = context.GetArrayStorageConst<CTime>();
    SCAssetManager const& scAssetManager = context.GetSingletonConst<SCAssetManager>();
    AssetManager const& assetManager = *scAssetManager.GetAssetManager();

	// Write Dependencies
    auto& renderStorage = context.GetArrayStorage<CRender>();
    auto& animStorage = context.GetArrayStorage<CAnimation>();
    SCRenderer& scRenderer = context.GetSingleton<SCRenderer>();
	Renderer& renderer = *scRenderer.GetRenderer();

    GridSpriteSheet const* statusEffectsSpriteSheet = assetManager.Get<GridSpriteSheet>(scRenderer.m_statusEffectsSpriteSheet);
	SpriteAnimationDef const* burnAnimDef = statusEffectsSpriteSheet ? statusEffectsSpriteSheet->GetAnimationDef("burn") : nullptr;
	SpriteAnimationDef const* hasteAnimDef = statusEffectsSpriteSheet ? statusEffectsSpriteSheet->GetAnimationDef("haste") : nullptr;
	SpriteAnimationDef const* slowAnimDef = statusEffectsSpriteSheet ? statusEffectsSpriteSheet->GetAnimationDef("slow") : nullptr;

    InstanceBufferID iboID = scRenderer.m_instancesPerSpriteSheet[scRenderer.m_statusEffectsSpriteSheet];
    InstanceBuffer* ibo = renderer.GetInstanceBuffer(iboID);
    ASSERT_OR_DIE(ibo != nullptr, "SRenderEntities::Run - Invalid instance buffer.");

    for (auto it = context.Iterate<CHealth, CRender, CAnimation>(); it.IsValid(); ++it)
    {
        CRender& render = renderStorage[it];
        if (!render.GetIsInCameraView())
        {
            continue;
        }

        CHealth const& health = healthStorage[it];

        // Tint the entity green based on poison
        float poisonSaturation = health.GetPoisonSaturation();
        render.m_tint = Rgba8::Lerp(render.m_baseTint, Rgba8::Green, poisonSaturation);

        // Render a sprite underlay for burning entities
        float burnSaturation = health.GetBurnSaturation();
		if (burnSaturation > 0.f)
		{
            CAnimation& anim = animStorage[it];

            if (!anim.m_burnInstance.IsValid() && burnAnimDef != nullptr)
            {
                anim.m_burnInstance.ChangeDef(*burnAnimDef, true);
            }

            if (anim.m_burnInstance.IsValid() && burnAnimDef != nullptr)
            {
                anim.m_burnInstance.Update(context.m_deltaSeconds);

                SpriteInstance burnInstance;
                burnInstance.m_dims = Vec2(render.m_renderRadius * 2.f, render.m_renderRadius * 2.f) * burnSaturation;
                burnInstance.m_position = Vec3(render.GetRenderPosition(), RenderConstants::s_burnSpriteRenderDepth);
                burnInstance.m_spriteIndex = anim.m_burnInstance.GetCurrentSpriteIndex();
                burnInstance.m_outlineRgba = Rgba8::TransparentBlack;
                burnInstance.m_indoorLight = 255;
                burnInstance.m_outdoorLight = 255;
                burnInstance.m_orientation = 17.f * static_cast<float>(it.m_currentIndex);

                ibo->AddInstance(burnInstance);
            }
		}
    }

    for (auto it = context.Iterate<CTime, CRender, CAnimation>(); it.IsValid(); ++it)
    {
        CRender& render = renderStorage[it];
        if (!render.GetIsInCameraView())
        {
            continue;
        }

		CTime const& time = timeStorage[it];
        if (time.IsHasted())
        {
            CAnimation& anim = animStorage[it];

            if (!anim.m_hasteInstance.IsValid() && hasteAnimDef != nullptr)
            {
                anim.m_hasteInstance.ChangeDef(*hasteAnimDef, true);
            }

            if (anim.m_hasteInstance.IsValid() && hasteAnimDef != nullptr)
            {
                anim.m_hasteInstance.Update(context.m_deltaSeconds);

                SpriteInstance hasteInstance;
                hasteInstance.m_dims = Vec2(render.m_renderRadius * 2.f, render.m_renderRadius * 2.f);
                hasteInstance.m_position = Vec3(render.GetRenderPosition(), RenderConstants::s_hasteSpriteRenderDepth);
                hasteInstance.m_spriteIndex = anim.m_hasteInstance.GetCurrentSpriteIndex();
                hasteInstance.m_outlineRgba = Rgba8::TransparentBlack;
                hasteInstance.m_indoorLight = 255;
                hasteInstance.m_outdoorLight = 255;
                hasteInstance.m_orientation = 0.f;

                ibo->AddInstance(hasteInstance);
            }
        }

        if (time.IsSlowed())
        {
			CAnimation& anim = animStorage[it];
			if (!anim.m_slowInstance.IsValid() && slowAnimDef != nullptr)
			{
				anim.m_slowInstance.ChangeDef(*slowAnimDef, true);
			}
            if (anim.m_slowInstance.IsValid() && slowAnimDef != nullptr)
            {
                anim.m_slowInstance.Update(context.m_deltaSeconds);

                SpriteInstance slowInstance;
                slowInstance.m_dims = Vec2(render.m_renderRadius * 2.f, render.m_renderRadius * 2.f);
                slowInstance.m_position = Vec3(render.GetRenderPosition(), RenderConstants::s_slowSpriteRenderDepth);
                slowInstance.m_spriteIndex = anim.m_slowInstance.GetCurrentSpriteIndex();
                slowInstance.m_outlineRgba = Rgba8::TransparentBlack;
                slowInstance.m_indoorLight = 255;
                slowInstance.m_outdoorLight = 255;
                slowInstance.m_orientation = 0.f;

                ibo->AddInstance(slowInstance);
            }
        }
    }
}


// Update Status Effect animations
// GridSpriteSheet const* statusEffectsSpriteSheet = assetManager.Get<GridSpriteSheet>(anim.m_statusEffectsSpriteSheet);
// if (statusEffectsSpriteSheet)
// {
//     if (SpriteAnimationDef const* burnAnimDef = statusEffectsSpriteSheet->GetAnimationDef("burn"))
//     {
//         anim.m_burnInstance.ChangeDef(*burnAnimDef);
//         anim.m_burnInstance.Update(context.m_deltaSeconds);
//     }
//     if (SpriteAnimationDef const* hasteAnimDef = statusEffectsSpriteSheet->GetAnimationDef("haste"))
//     {
//         anim.m_hasteInstance.ChangeDef(*hasteAnimDef);
//         anim.m_hasteInstance.Update(context.m_deltaSeconds);
//     }
// }