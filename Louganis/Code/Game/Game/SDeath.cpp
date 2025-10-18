// Bradley Christensen - 2022-2025
#include "SDeath.h"
#include "CAnimation.h"
#include "CHealth.h"
#include "CLifetime.h"
#include "SCEntityFactory.h"
#include "Engine/Assets/AssetManager.h"
#include "Engine/Assets/GridSpriteSheet.h"
#include "Engine/Debug/DevConsoleUtils.h"



//----------------------------------------------------------------------------------------------------------------------
void SDeath::Startup()
{
    AddReadDependencies<CHealth>();
	AddWriteDependencies<CAnimation, CLifetime, SCEntityFactory>();
}



//----------------------------------------------------------------------------------------------------------------------
void SDeath::Run(SystemContext const& context)
{
	auto& healthStorage = g_ecs->GetArrayStorage<CHealth>();
	auto& lifetimeStorage = g_ecs->GetArrayStorage<CLifetime>();
	auto& animStorage = g_ecs->GetArrayStorage<CAnimation>();
	auto& entityFactory = g_ecs->GetSingleton<SCEntityFactory>();

	for (auto it = g_ecs->Iterate<CHealth, CLifetime>(context); it.IsValid(); ++it)
	{
		CHealth const& health = healthStorage[it];
		if (!health.GetIsDead())
		{
			continue;
		}


		CAnimation& animation = animStorage[it];
		GridSpriteSheet* gridSpriteSheet = g_assetManager->Get<GridSpriteSheet>(animation.m_gridSpriteSheet);
		if (!gridSpriteSheet)
		{
			DevConsoleUtils::LogWarning("SDeath::Run - Entity has no valid GridSpriteSheet for death animation, destroying entity immediately.");
			entityFactory.m_entitiesToDestroy.push_back(it.m_currentIndex);
			continue;
		}

		SpriteAnimationDef const* deathAnim = gridSpriteSheet->GetAnimationDef("Death");
		if (!deathAnim)
		{
			DevConsoleUtils::LogWarning("SDeath::Run - Entity has no valid death animation, destroying entity immediately.");
			entityFactory.m_entitiesToDestroy.push_back(it.m_currentIndex);
			continue;
		}

		SpriteAnimationDef const& currentAnim = animation.m_animInstance.GetDef();
		if (currentAnim.GetName() != deathAnim->GetName())
		{
			animation.m_animInstance = deathAnim->MakeAnimInstance(0, 1);
		}
		else if(animation.m_animInstance.IsFinished())
		{
			CLifetime& lifetime = lifetimeStorage[it];
			if (lifetime.m_lifetime < 0.f)
			{
				lifetime.SetLifetime(5.f);
			}
		}
	}
}
