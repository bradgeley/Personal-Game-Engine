// Bradley Christensen - 2022-2025
#pragma once
#include "Engine/Math/RandomNumberGenerator.h"



class RandomNumberGenerator;



//----------------------------------------------------------------------------------------------------------------------
class SCRandomNumberGenerator
{
public:

	RandomNumberGenerator const* GetRNG() const	{ return m_rng; }
	RandomNumberGenerator* GetRNG()				{ return m_rng; }
	void SetRNG(RandomNumberGenerator* rng)		{ m_rng = rng; }

private:

	RandomNumberGenerator* m_rng = nullptr;
};