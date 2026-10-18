// Bradley Christensen - 2022-2026
#pragma once
#include "Engine/ECS/EntityID.h"



struct AbilityDef;
struct CAbility;
struct CPlaceable;
struct EntityDebugContext;
struct SystemContext;
struct Vec2;
class VertexBuffer;



//----------------------------------------------------------------------------------------------------------------------
class Ability
{
public:

	Ability(AbilityDef const& def);
	virtual ~Ability() = default;

	virtual void Initialize(SystemContext const& context, EntityID ownerEntityID);

	virtual void Update(SystemContext const& context, CAbility const& ability, Vec2 const& location, float timeDilation = 1.f) = 0;
	virtual void Render(SystemContext const& context, CAbility const& ability, Vec2 const& location) const;
	virtual Ability* DeepCopy() const = 0;
	virtual void CopyTransientDataTo(Ability& other) const;
	virtual void AddDebugVerts(VertexBuffer& out_vbo, SystemContext const& context, CAbility const& ability) const;
	virtual void AddDebugVertsAtLocation(VertexBuffer& out_vbo, SystemContext const& context, CAbility const& ability, Vec2 const& location) const;
	virtual void AppendDebugString(EntityDebugContext& debugContext) const;

public:

	EntityID m_owner = EntityID::Invalid;
	AbilityDef const* m_abilityDef = nullptr;
};