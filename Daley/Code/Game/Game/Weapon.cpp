// Bradley Christensen - 2022-2025
#include "Weapon.h"
#include "EntityDef.h"
#include "WeaponDef.h"
#include "Engine/Debug/DevConsoleUtils.h"



//----------------------------------------------------------------------------------------------------------------------
ProjectileHitWeapon::ProjectileHitWeapon(ProjectileHitWeaponDef const& def)
{
    m_weaponDef = &def;
    m_projectileDef = EntityDef::GetEntityDef(def.m_projectileDefName);
    if (!m_projectileDef)
    {
        DevConsoleUtils::LogError("ProjectileHitWeaponDef::MakeBaseWeapon - Failed to find projectile EntityDef for name: %s", def.m_projectileDefName.ToCStr());
    }
    m_damage = def.m_baseDamage;
    m_attacksPerSecond = def.m_baseAttacksPerSecond;
    m_projSpeedUnitsPerSec = def.m_baseProjSpeedUnitsPerSec;
}



//----------------------------------------------------------------------------------------------------------------------
void ProjectileHitWeapon::Update(float deltaSeconds)
{

}



//----------------------------------------------------------------------------------------------------------------------
Weapon* ProjectileHitWeapon::DeepCopy() const
{
    ProjectileHitWeapon* copy = new ProjectileHitWeapon(*reinterpret_cast<ProjectileHitWeaponDef const*>(m_weaponDef));
    *copy = *this;
    return copy;
}
