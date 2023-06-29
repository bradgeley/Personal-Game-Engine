// Bradley Christensen - 2022-2023
#include "Engine/Math/Noise.h"
#include "Engine/Math/MathUtils.h"



//----------------------------------------------------------------------------------------------------------------------
unsigned int GetRawNoise1D(int x, int seed)
{
	// todo: test thoroughly for weird patterns
	unsigned int bits = (unsigned int) x;

	// Randomly generated bits
	int RANDOM_BITS_1 = 0b0110'0011'1110'1000'0111'1110'1010'0110;
	int RANDOM_BITS_2 = 0b1111'0100'0000'0101'0100'0010'1111'0110;
	int RANDOM_BITS_3 = 0b0011'1101'0110'1001'1010'1101'1111'1111;

	// todo: Test/research other ways of combining these values
	bits += seed;
	bits *= RANDOM_BITS_1;
	bits *= RANDOM_BITS_2;
	bits *= RANDOM_BITS_3;

	return bits;
}



//----------------------------------------------------------------------------------------------------------------------
unsigned int GetRawNoise2D(int x, int y, int seed)
{
	int PRIME_NUMBER = 87; // todo: test different primes
	return GetRawNoise1D(x + (PRIME_NUMBER * y), seed);
}



//----------------------------------------------------------------------------------------------------------------------
float GetNoiseZeroToOne1D(int x, int seed)
{
	unsigned int rawNoise = GetRawNoise1D(x, seed);
	float zeroToOne = (double) rawNoise * (1.0 / (double) 0xFFFF'FFFF);
	return zeroToOne;
}



//----------------------------------------------------------------------------------------------------------------------
float GetNoiseZeroToOne2D(int x, int y, int seed)
{
	unsigned int rawNoise = GetRawNoise2D(x, y, seed);
	float zeroToOne = (double) rawNoise * (1.0 / (double) 0xFFFF'FFFF);
	return zeroToOne;
}



//----------------------------------------------------------------------------------------------------------------------
float GetFractalNoise1D(float position, float scale, unsigned int numOctaves, float octavePersistence, float octaveScale, bool renormalize, unsigned int seed)
{
	float offset = 1.f; // todo: use irrational offset like squirrel

	float relativePos = position * (1.f / scale);
	float relativeFloor = FloorF(position);
	int indexWest = static_cast<int>(relativeFloor);
	int indexEast = indexWest + 1;

	// todo: octaves

	float noiseWest = GetNoiseZeroToOne1D(indexWest, seed);
	float noiseEast = GetNoiseZeroToOne1D(indexEast, seed);

	// Weighted average of the 2 to get the noise for this position
	float distanceToWest = relativePos - relativeFloor;
	float weightEast = distanceToWest; // todo: float weightEast = SmoothStep3(distanceToWest); // East weight inc as distance from west inc
	float weightWest = 1.f - weightEast;
	float noiseZeroToOne = weightWest * noiseWest + weightEast * noiseEast;
	float noiseNegOneToOne = (noiseZeroToOne - 0.5f) * 2.f; // normalize to -1,1


	// Todo: renormalize


	return noiseNegOneToOne;
}
