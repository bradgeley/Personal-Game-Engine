// Bradley Christensen - 2023
#include "PlanetGenerator.h"
#include "EntityDef.h"
#include "Engine/Renderer/Texture.h"
#include "Engine/Renderer/Rgba8.h"
#include "Engine/Math/Noise.h"
#include "Game/SEntityFactory.h"
#include "Game/CRender.h"



//----------------------------------------------------------------------------------------------------------------------
EntityID PlanetGenerator::Generate() const
{
	SCEntityFactory* factory = g_ecs->GetComponent<SCEntityFactory>();
	auto def = factory->GetEntityDef("BasePlanet");
	auto result = SEntityFactory::CreateEntityFromDef(def);

	auto& rc = *g_ecs->GetComponent<CRender>(result);

	// Generate background texture
	rc.m_texture = new Texture();
	rc.m_texture->CreateUniformTexture(IntVec2(1024, 1024), Rgba8::White);

	return result;
}
