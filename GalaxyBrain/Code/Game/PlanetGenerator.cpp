// Bradley Christensen - 2023
#include "PlanetGenerator.h"
#include "EntityDef.h"
#include "Engine/Renderer/Texture.h"
#include "Engine/Renderer/Rgba8.h"
#include "Engine/Core/Image.h"
#include "Engine/Core/StringUtils.h"
#include "Engine/Math/Noise.h"
#include "Engine/Math/MathUtils.h"
#include "Engine/Math/RandomNumberGenerator.h"
#include "Game/SEntityFactory.h"
#include "Game/CRender.h"



//----------------------------------------------------------------------------------------------------------------------
EntityDef const* PlanetGenerator::Generate(int seed)
{
	SCEntityFactory* factory = g_ecs->GetComponent<SCEntityFactory>();
	auto def = factory->GetEntityDef("BasePlanet");

	static int numGenPlanets = 0;
	++numGenPlanets;
	std::string planetName = StringF("genPlanet_%i", numGenPlanets);

	// Create the new gen'd planet's definition
	EntityDef*& result = factory->m_entityDefinitions[planetName];
	result = new EntityDef();
	*result = *def; // copy base planet data first

	// Randomize size of planet
	g_rng->SetSeed(seed);
	float radius = 10'000.f;
	float radiusSquared = radius * radius;
	result->m_collision->m_radius = radius;
	result->m_render->m_scale = radius;
	result->m_physics->m_mass = radius;
	result->m_physics->m_angularVelocity = g_rng->PlusOrMinus() * g_rng->GetRandomFloatInRange(5.f, 10.f);
	float terrainNoiseSize = 500.f;

	// Randomize other planet factors
	float seaLevel		= (GetPerlinNoise1D(seed) + 1.f) * 0.5f; // [0,1]
	float snowcapLevel	= (GetPerlinNoise1D(seed + 1) + 1.f) * 0.5f; // [0,1]
	snowcapLevel		= RangeMap(snowcapLevel, -1.f, 1.f, seaLevel, 1.f); // [seaLevel, 1.f]
	float cloudiness	= (GetPerlinNoise1D(seed + 2) + 1.f) * 0.5f; // [0,1]

	// Generate unique planet background texture
	Image image;
	IntVec2 imageDims = IntVec2(1024, 1024);
	Vec2 imageCenter = Vec2(imageDims.x * 0.5f, imageDims.y * 0.5f);
	float radiusInImagePixels = imageCenter.x;
	float radiusInImagePixelsSquared = (radiusInImagePixels + 1) * (radiusInImagePixels + 1); // Add 1 for buffer so we don't see transparent black pixels on edges
	image.Initialize(IntVec2(1024, 1024), Rgba8::TransparentBlack);
	for (int y = 0; y < image.GetHeight(); ++y)
	{
		for (int x = 0; x < image.GetWidth(); ++x)
		{
			float distSquared = (Vec2(x, y) - imageCenter).GetLengthSquared();
			if (distSquared > radiusInImagePixelsSquared)
			{
				// Don't bother generating noise for the edges of the texture
				continue;
			}

			float terrainHeight = (GetPerlinNoise2D(x, y, terrainNoiseSize, 8, 0.5f, 2.f, true, seed + 10) + 1.f) * 0.5f; // [0,1]
			terrainHeight = SmoothStep3(terrainHeight);

			Rgba8 terrainTint;
			if (terrainHeight < seaLevel)
			{
				float seaDepth = RangeMap(terrainHeight, 0.f, 1.f, 0.f, terrainHeight);
				terrainTint = Rgba8::Lerp(Rgba8::DarkOceanBlue, Rgba8::LightOceanBlue, seaDepth);
			}
			else
			{
				float mountainHeight = RangeMap(terrainHeight, 0.f, 1.f, terrainHeight, 1.f);
				terrainTint = Rgba8::Lerp(Rgba8::Rgba8::Green, Rgba8::DarkGray, mountainHeight);
			}
			if (terrainHeight > snowcapLevel)
			{
				terrainTint = Rgba8::White;
			}

			Rgba8 finalTint = terrainTint;
			image.Set(x, y, finalTint);
		}
	}

	result->m_render->m_texture = new Texture();
	result->m_render->m_texture->CreateFromImage(image);

	return result;
}
