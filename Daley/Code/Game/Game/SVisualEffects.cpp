// Bradley Christensen - 2022-2026
#include "SVisualEffects.h"
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
void SVisualEffects::Startup()
{
    AddWriteAllDependencies(); // Spawns entities
}



//----------------------------------------------------------------------------------------------------------------------
void SVisualEffects::Run(SystemContext const& context) const
{
    // Read Dependencies
    auto const& healthStorage = context.GetArrayStorageConst<CHealth>();
    auto const& transStorage = context.GetArrayStorageConst<CTransform>();

	// Write Dependencies
    auto& renderStorage = context.GetArrayStorage<CRender>();
	auto& attachStorage = context.GetArrayStorage<CAttachment>();
	auto& factory = context.GetSingleton<SCEntityFactory>();
    // g_renderer

    // Push back an instance for every entity in camera view this frame
    for (auto it = context.Iterate<CHealth, CRender, CAttachment, CTransform>(); it.IsValid(); ++it)
    {
        CRender& render = renderStorage[it];
        if (!render.GetIsInCameraView())
        {
            continue;
        }

        CAttachment& attachment = attachStorage[it];
        CHealth const& health = healthStorage[it];

		float poisonSaturation = health.GetPoisonSaturation();
        render.m_tint = Rgba8::Lerp(render.m_baseTint, Rgba8::Green, poisonSaturation);

		float burnSaturation = health.GetBurnSaturation();

        if (burnSaturation > 0.f)
        {
            if (!g_ecs->IsValid(attachment.m_attachedBurnVFX))
            {
                CTransform const& transform = transStorage[it];
                // Spawn and attach the vfx on the entity side
                SpawnInfo spawnInfo;
                spawnInfo.m_def = EntityDef::GetEntityDef("BurnUnderlay");
                spawnInfo.m_spawnPos = transform.m_pos;
                spawnInfo.m_spawnOrientation = transform.m_orientation;
                attachment.m_attachedBurnVFX = SEntityFactory::SpawnEntity(context, spawnInfo);
            }

            if (g_ecs->IsValid(attachment.m_attachedBurnVFX))
            {
                // Attach the vfx on the vfx side
				CAttachment& burnVFXAttachment = attachStorage[attachment.m_attachedBurnVFX];
                burnVFXAttachment.m_attachedTo = it.GetEntityID();
                burnVFXAttachment.m_destroyIfAttachedToEntityDestroyed = true;

                // Update burn effect position and scale
				CRender& burnEffectRender = renderStorage[attachment.m_attachedBurnVFX];
                burnEffectRender.m_renderRadius = render.m_renderRadius * burnSaturation;
            }
        }
        else
        {
            if (g_ecs->IsValid(attachment.m_attachedBurnVFX))
            {
                // Destroy the entity, and sever the attachment on the entity side
				factory.m_entitiesToDestroy.push_back(attachment.m_attachedBurnVFX);
				attachment.m_attachedBurnVFX = EntityID::Invalid;
            }
        }
    }
}
