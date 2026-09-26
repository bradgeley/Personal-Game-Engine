// Bradley Christensen - 2022-2026
#include "Ability.h"
#include "AbilityDef.h"
#include "EntityDebugContext.h"
#include "Engine/Core/ErrorUtils.h"
#include "Engine/Core/StringUtils.h"
#include "Engine/ECS/SystemContext.h"



//----------------------------------------------------------------------------------------------------------------------
uint32_t Ability::s_splashIdCounter = 0;



//----------------------------------------------------------------------------------------------------------------------
Ability::Ability(AbilityDef const& def) : m_abilityDef(&def)
{
}



//----------------------------------------------------------------------------------------------------------------------
void Ability::Initialize(SystemContext const& context, EntityID ownerEntityID)
{
    ASSERT_OR_DIE(context.IsValid(ownerEntityID), "Ability::Initialize - owner entity is invalid.");
    m_owner = ownerEntityID;
}



//----------------------------------------------------------------------------------------------------------------------
void Ability::Render(SystemContext const&, CAbility const&, Vec2 const&) const
{

}



//----------------------------------------------------------------------------------------------------------------------
void Ability::AppendDebugString(EntityDebugContext& debugContext) const
{
    debugContext.m_debugString += StringUtils::StringF("Ability: %s\n", m_abilityDef ? m_abilityDef->m_name.ToCStr() : "Invalid");
}