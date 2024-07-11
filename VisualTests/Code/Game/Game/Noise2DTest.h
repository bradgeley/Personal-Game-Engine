// Bradley Christensen - 2023
#pragma once
#include "VisualTest.h"
#include "Engine/Renderer/Rgba8.h"
#include "Engine/Math/Grid2D.h"



//----------------------------------------------------------------------------------------------------------------------
// Tests noise functions
//
class Noise2DTest : public VisualTest
{
public:

	Noise2DTest() { m_name = "Noise2D Test"; }

	void Startup() override;
	void Update(float deltaSeconds) override;
	void Render() const override;
	void Shutdown() override;

	void GenerateNoise(int seed);
	Rgba8 GetColorForNoise(float noiseValue);

	int m_seed = 0;
	Grid2D<Rgba8> m_rawNoise2D;
	Grid2D<Rgba8> m_fractalNoise2D;
	Grid2D<Rgba8> m_perlinNoise2D;
};

