// Bradley Christensen - 2022-2026
#include "SRenderEffects.h"
#include "CAttachment.h"
#include "CHealth.h"
#include "CRender.h"
#include "CTransform.h"
#include "EntityDef.h"
#include "SEntityFactory.h"
#include "SCEntityFactory.h"
#include "SpawnInfo.h"
#include "Engine/Assets/AssetManager.h"
#include "Engine/Assets/GridSpriteSheet.h"
#include "Engine/Renderer/Renderer.h"
#include "Engine/Renderer/Vertex_PCU.h"
#include "Engine/Renderer/VertexUtils.h"



//----------------------------------------------------------------------------------------------------------------------
void SRenderEffects::Startup()
{
    AddWriteAllDependencies(); // Spawns entities
}



//----------------------------------------------------------------------------------------------------------------------
void SRenderEffects::Run(SystemContext const& context)
{
    auto& renderStorage = g_ecs->GetArrayStorage<CRender>();
    auto& healthStorage = g_ecs->GetArrayStorage<CHealth>();
	auto& attachStorage = g_ecs->GetMapStorage<CAttachment>();
	auto& factory = g_ecs->GetSingleton<SCEntityFactory>();

	BitMask vfxBitMask = g_ecs->GetComponentBitMask<CAttachment>();

    // Push back an instance for every entity in camera view this frame
    for (auto it = g_ecs->Iterate<CHealth, CRender>(context); it.IsValid(); ++it)
    {
        CRender& render = *renderStorage.Get(it);
        if (!render.GetIsInCameraView())
        {
            continue;
        }

        CHealth const& health = *healthStorage.Get(it);

		float poisonSaturation = health.GetPoisonSaturation();
        render.m_tint = Rgba8::Lerp(render.m_baseTint, Rgba8::Green, poisonSaturation);

		float burnSaturation = health.GetBurnSaturation();

        CAttachment* attachment;
        if (g_ecs->DoesEntityHaveComponentsUnsafe(it.m_currentIndex, vfxBitMask))
        {
            attachment = attachStorage.Get(it);
        }
        else
        {
            attachment = g_ecs->AddComponent<CAttachment>(it.GetEntityID());
        }

        if (burnSaturation > 0.f)
        {
            if (!g_ecs->IsValid(attachment->m_attachedBurnVFX))
            {
                // Spawn and attach the vfx on the entity side
                SpawnInfo spawnInfo;
                spawnInfo.m_def = EntityDef::GetEntityDef("BurnEffect");
                attachment->m_attachedBurnVFX = SEntityFactory::SpawnEntity(spawnInfo);
            }

            if (g_ecs->IsValid(attachment->m_attachedBurnVFX))
            {
                // Attach the vfx on the vfx side
                CAttachment* burnAttachment = g_ecs->AddComponent<CAttachment>(attachment->m_attachedBurnVFX);
                burnAttachment->m_attachedTo = it.GetEntityID();
                burnAttachment->m_destroyIfAttachedToEntityDestroyed = true;

                // Update burn effect position and scale
				CRender* burnEffectRender = g_ecs->GetComponent<CRender>(attachment->m_attachedBurnVFX);
                if (burnEffectRender)
                {
                    burnEffectRender->m_renderRadius = render.m_renderRadius * burnSaturation * 0.8f;
                }
            }
        }
        else
        {
            if (g_ecs->IsValid(attachment->m_attachedBurnVFX))
            {
                // Destroy the entity, and sever the attachment on the entity side
				factory.m_entitiesToDestroy.push_back(attachment->m_attachedBurnVFX);
				attachment->m_attachedBurnVFX = EntityID::Invalid;
            }
        }
    }
}



//----------------------------------------------------------------------------------------------------------------------
void SRenderEffects::Shutdown()
{

}
