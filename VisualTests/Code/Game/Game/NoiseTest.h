// Bradley Christensen - 2023
#pragma once
#include "VisualTest.h"
#include <vector>



//----------------------------------------------------------------------------------------------------------------------
// Tests noise functions
//
class NoiseTest : public VisualTest
{
public:

	void Startup() override;
	void Update(float deltaSeconds) override;
	void Render() const override;
	void Shutdown() override;

	void GenerateNoise(int seed);

	int m_seed = 0;
	std::vector<float> m_rawNoise1D;
	std::vector<float> m_fractalNoise1D;
	std::vector<float> m_perlinNoise1D;
};

