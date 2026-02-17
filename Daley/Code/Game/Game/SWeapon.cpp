// Bradley Christensen - 2022-2025
#include "SWeapon.h"
#include "CTransform.h"
#include "CWeapon.h"
#include "WeaponDef.h"



//----------------------------------------------------------------------------------------------------------------------
void SWeapon::Startup()
{
	WeaponDef::LoadFromXML();

	AddWriteAllDependencies();
}



//----------------------------------------------------------------------------------------------------------------------
void SWeapon::Run(SystemContext const& context)
{
	auto& weaponStorage = g_ecs->GetMapStorage<CWeapon>();
	auto& transStorage = g_ecs->GetArrayStorage<CTransform>();

	for (auto it = g_ecs->Iterate<CWeapon, CTransform>(context); it.IsValid(); ++it)
	{
		CWeapon& weapon = *weaponStorage.Get(it);
		CTransform const& transform = *transStorage.Get(it);

		for (Weapon* w : weapon.m_weapons)
		{
			w->Update(context.m_deltaSeconds, transform.m_pos);
		}
	}
}



//----------------------------------------------------------------------------------------------------------------------
void SWeapon::Shutdown()
{
	WeaponDef::Shutdown();
}
