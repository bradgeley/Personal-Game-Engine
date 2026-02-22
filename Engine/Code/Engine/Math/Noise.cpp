// Bradley Christensen - 2022-2026
#include "Engine/Math/Noise.h"
#include "Engine/Math/MathUtils.h"
#include "Engine/Math/Vec2.h"
#include "Engine/Math/IntVec2.h"
#include "Engine/Math/AABB2.h"



//----------------------------------------------------------------------------------------------------------------------
unsigned int GetRawNoise1D(int x, int seed)
{
	unsigned int bits = (unsigned int) x;

	// Randomly generated bits
	unsigned int RANDOM_BITS_1 = 2440335539;
	unsigned int RANDOM_BITS_2 = 3143968388;
	unsigned int RANDOM_BITS_3 = 3939586176;

	bits *= RANDOM_BITS_1;
	bits ^= bits >> 13;
	bits += seed;
	bits += RANDOM_BITS_2;
	bits ^= bits >> 19;
	bits *= RANDOM_BITS_3;
	bits ^= bits >> 23;

	return bits;
}



//----------------------------------------------------------------------------------------------------------------------
unsigned int GetRawNoise2D(int x, int y, int seed)
{
	int PRIME_NUMBER = 198491317; // todo: test different primes?
	return GetRawNoise1D(x + (PRIME_NUMBER * y), seed);
}



//----------------------------------------------------------------------------------------------------------------------
size_t GetRawNoise1D_64(int x, size_t seed)
{
	size_t bits = static_cast<size_t>(x);

	// Randomly generated bits
	#if INTPTR_MAX == INT64_MAX
		size_t RANDOM_BITS_1 = 16069955140179554870;
		size_t RANDOM_BITS_2 = 12176651035439328948;
		size_t RANDOM_BITS_3 = 15364385324645250847;
	#else
		size_t RANDOM_BITS_1 = 2440335539;
		size_t RANDOM_BITS_2 = 3143968388;
		size_t RANDOM_BITS_3 = 3939586176;
	#endif

	bits *= RANDOM_BITS_1;
	bits ^= bits >> 13;
	bits += seed;
	bits += RANDOM_BITS_2;
	bits ^= bits >> 19;
	bits *= RANDOM_BITS_3;
	bits ^= bits >> 23;

	return bits;
}



//----------------------------------------------------------------------------------------------------------------------
float GetNoiseZeroToOne1D(int x, int seed)
{
	unsigned int rawNoise = GetRawNoise1D(x, seed);
	double zeroToOne = (double) rawNoise * (1.0 / (double) 0xFFFF'FFFF);
	return (float) zeroToOne;
}



//----------------------------------------------------------------------------------------------------------------------
float GetNoiseZeroToOne2D(int x, int y, int seed)
{
	unsigned int rawNoise = GetRawNoise2D(x, y, seed);
	double zeroToOne = (double) rawNoise * (1.0 / (double) 0xFFFF'FFFF);
	return (float) zeroToOne;
}



//----------------------------------------------------------------------------------------------------------------------
float GetFractalNoise1D(float position, float scale, unsigned int numOctaves, float octavePersistence, float octaveScale, bool renormalize, unsigned int seed)
{
	constexpr float OCTAVE_OFFSET = 0.636764989593174f; // Translation/bias to add to each octave

	// Scale down the position if scale is very large, so that you traverse the noise slower. Steps of 1 become steps of 0.1 on scale 10
	float relativePos = position * (1.f / scale) + OCTAVE_OFFSET;

	float totalNoise = 0.f;
	float persistence = 1.f;
	float maxAmplitude = 0.f; // Each octave adds some theoretical max amplitude, usually starts at 1 then 1.5 then 1.75, for 3 octaves, etc.

	for (unsigned int octaveIndex = 0; octaveIndex < numOctaves; ++octaveIndex)
	{
		// Weighted average of the 2 to get the noise for this position
		float relativeFloor = (float) MathUtils::FloorF(relativePos);
		float distanceToWest = relativePos - relativeFloor;

		int indexWest = static_cast<int>(relativeFloor);
		int indexEast = indexWest + 1;

		float noiseWest = GetNoiseZeroToOne1D(indexWest, seed);
		float noiseEast = GetNoiseZeroToOne1D(indexEast, seed);

		float weightEast = MathUtils::SmoothStep3(distanceToWest); // East weight inc as distance from west inc
		float weightWest = 1.f - weightEast;
		float noiseZeroToOne = weightWest * noiseWest + weightEast * noiseEast;
		float noiseNegOneToOne = (noiseZeroToOne - 0.5f) * 2.f; // map to -1,1

		// Diminish by the octave persistence
		noiseNegOneToOne *= persistence;
		maxAmplitude += persistence;

		relativePos *= octaveScale;
		relativePos += OCTAVE_OFFSET;

		totalNoise += noiseNegOneToOne;

		// Set data for next iteration
		persistence *= octavePersistence;
		++seed;
	}

	if (renormalize)
	{
		totalNoise /= maxAmplitude; // Get back into range -1,1 from the range -A,A (where A = max amplitude from octaves)
		totalNoise = (totalNoise * 0.5f) + 0.5f;	// Map to 0,1 for smooth step
		totalNoise = MathUtils::SmoothStep3(totalNoise);		// Octaves tend to make noise middly, this pushes edges towards -1 and 1 again
		totalNoise = (totalNoise * 2.0f) - 1.f;		// Map back to -1,1
	}

	return totalNoise;
}



//----------------------------------------------------------------------------------------------------------------------
float GetFractalNoise2D(float x, float y, float scale, unsigned int numOctaves, float octavePersistence, float octaveScale, bool renormalize, unsigned int seed)
{
	static Vec2 OCTAVE_OFFSET = Vec2(0.636764989593174f, 0.636764989593174f); // Translation/bias to add to each octave
	Vec2 relativePos = Vec2(x, y) * (1.f / scale) + OCTAVE_OFFSET;

	float totalNoise = 0.f;
	float persistence = 1.f;
	float maxAmplitude = 0.f; // Each octave adds some theoretical max amplitude, usually starts at 1 then 1.5 then 1.75, for 3 octaves, etc.

	for (unsigned int octaveIndex = 0; octaveIndex < numOctaves; ++octaveIndex)
	{
		Vec2 relativeFloor = Vec2(MathUtils::FloorF(relativePos.x), MathUtils::FloorF(relativePos.y));

		IntVec2 southWest = IntVec2(relativeFloor);
		IntVec2 northEast = southWest + IntVec2(1, 1);

		float southWestNoise = GetNoiseZeroToOne2D(southWest.x, southWest.y, seed);
		float northEastNoise = GetNoiseZeroToOne2D(northEast.x, northEast.y, seed);
		float northWestNoise = GetNoiseZeroToOne2D(southWest.x, northEast.y, seed);
		float southEastNoise = GetNoiseZeroToOne2D(northEast.x, southWest.y, seed);

		Vec2 distanceToSouthWest = relativePos - relativeFloor;
		float weightEast = MathUtils::SmoothStep3(distanceToSouthWest.x);
		float weightNorth = MathUtils::SmoothStep3(distanceToSouthWest.y);
		float weightWest = 1.f - weightEast;
		float weightSouth = 1.f - weightNorth;

		float blendNorth = weightEast * northEastNoise + weightWest * northWestNoise;
		float blendSouth = weightEast * southEastNoise + weightWest * southWestNoise;
		float noiseZeroToOne = weightNorth * blendNorth + weightSouth * blendSouth;
		float noiseNegOneToOne = (noiseZeroToOne - 0.5f) * 2.f; // map to -1,1

		// Diminish by the octave persistence
		noiseNegOneToOne *= persistence;
		maxAmplitude += persistence;

		relativePos *= octaveScale;
		relativePos += OCTAVE_OFFSET;

		totalNoise += noiseNegOneToOne;

		// Set data for next iteration
		persistence *= octavePersistence;
		++seed;
	}

	if (renormalize)
	{
		totalNoise /= maxAmplitude; // Get back into range -1,1 from the range -A,A (where A = max amplitude from octaves)
		totalNoise = (totalNoise * 0.5f) + 0.5f;	// Map to 0,1 for smooth step
		totalNoise = MathUtils::SmoothStep3(totalNoise);		// Octaves tend to make noise middly, this pushes edges towards -1 and 1 again
		totalNoise = (totalNoise * 2.0f) - 1.f;		// Map back to -1,1
	}

	return totalNoise;
}



//----------------------------------------------------------------------------------------------------------------------
float GetPerlinNoise1D(float position, float scale, unsigned int numOctaves, float octavePersistence, float octaveScale, bool renormalize, unsigned int seed)
{
	constexpr float OCTAVE_OFFSET = 0.636764989593174f; // Translation/bias to add to each octave
	constexpr float gradients[2] = { -1.f, 1.f }; // Each point on the gradient line is either pointing left or right
	
	// Scale down the position if scale is very large, so that you traverse the noise slower. Steps of 1 become steps of 0.1 on scale 10
	float relativePos = (position + OCTAVE_OFFSET) * (1.f / scale);
	
	float totalNoise = 0.f;
	float persistence = 1.f;
	float maxAmplitude = 0.f; // Each octave adds some theoretical max amplitude, usually starts at 1 then 1.5 then 1.75, for 3 octaves, etc.
	
	for (unsigned int octaveIndex = 0; octaveIndex < numOctaves; ++octaveIndex)
	{
		// Weighted average of the 2 to get the noise for this position
		float relativeFloor = (float) MathUtils::FloorF(relativePos);
		float distanceToWest = relativePos - relativeFloor;
		float distanceToEast = distanceToWest - 1.f; // always negative
	
		int indexWest = static_cast<int>(relativeFloor);
		int indexEast = indexWest + 1;
	
		float gradientWest = gradients[GetRawNoise1D(indexWest, seed) & 0x00000001];
		float gradientEast = gradients[GetRawNoise1D(indexEast, seed) & 0x00000001];
	
		float dotWest = gradientWest * distanceToWest; // 1D "dot product"
		float dotEast = gradientEast * distanceToEast;
	
		float weightEast = MathUtils::SmoothStep3(distanceToWest); // East weight inc as distance from west inc
		float weightWest = 1.f - weightEast;
		float noiseZeroToOne = (weightWest * dotWest) + (weightEast * dotEast);
		float noiseThisOctave = 2.f * noiseZeroToOne; // 1D Perlin is in -.5,.5, map to -1,1
	
		// Diminish by the octave persistence
		noiseThisOctave *= persistence;
		maxAmplitude += persistence;
	
		relativePos *= octaveScale;
		relativePos += OCTAVE_OFFSET;
	
		totalNoise += noiseThisOctave;
	
		// Set data for next iteration
		persistence *= octavePersistence;
		++seed;
	}
	
	if (renormalize)
	{
		totalNoise /= maxAmplitude; // Get back into range -1,1 from the range -A,A (where A = max amplitude from octaves)
		totalNoise = (totalNoise * 0.5f) + 0.5f;	// Map to 0,1 for smooth step
		totalNoise = MathUtils::SmoothStep3(totalNoise);		// Octaves tend to make noise middly, this pushes edges towards -1 and 1 again
		totalNoise = (totalNoise * 2.0f) - 1.f;		// Map back to -1,1
	}
	
	return totalNoise;
}



//----------------------------------------------------------------------------------------------------------------------
float GetPerlinNoise2D(float x, float y, float scale, unsigned int numOctaves, float octavePersistence, float octaveScale, bool renormalize, unsigned int seed)
{
	static Vec2 OCTAVE_OFFSET = Vec2(0.636764989593174f, 0.636764989593174f); // Translation/bias to add to each octave
	static Vec2 const gradients[8] = // Normalized unit vectors in 8 quarter-cardinal directions
	{
		Vec2(+0.923879533f, +0.382683432f), //  22.5 degrees (ENE)
		Vec2(+0.382683432f, +0.923879533f), //  67.5 degrees (NNE)
		Vec2(-0.382683432f, +0.923879533f), // 112.5 degrees (NNW)
		Vec2(-0.923879533f, +0.382683432f), // 157.5 degrees (WNW)
		Vec2(-0.923879533f, -0.382683432f), // 202.5 degrees (WSW)
		Vec2(-0.382683432f, -0.923879533f), // 247.5 degrees (SSW)
		Vec2(+0.382683432f, -0.923879533f), // 292.5 degrees (SSE)
		Vec2(+0.923879533f, -0.382683432f)  // 337.5 degrees (ESE)
	};

	Vec2 sampleLocation = (Vec2(x, y) / scale) + OCTAVE_OFFSET;
	float totalNoise = 0.f;
	float persistence = 1.f;
	float maxAmplitude = 0.f; // Each octave adds some theoretical max amplitude, usually starts at 1 then 1.5 then 1.75, for 3 octaves, etc.

	for (unsigned int octaveIndex = 0; octaveIndex < numOctaves; ++octaveIndex)
	{
		AABB2 cell;
		cell.mins = sampleLocation.GetFloor();
		cell.maxs = cell.mins + Vec2(1.f, 1.f);
		Vec2 cellTopLeft = cell.GetTopLeft();
		Vec2 cellBotRight = cell.GetBottomRight();

		unsigned int noiseBotLeft = GetRawNoise2D((int) cell.mins.x, (int) cell.mins.y, seed);
		unsigned int noiseTopLeft = GetRawNoise2D((int) cellTopLeft.x, (int) cellTopLeft.y, seed);
		unsigned int noiseTopRight = GetRawNoise2D((int) cell.maxs.x, (int) cell.maxs.y, seed);
		unsigned int noiseBotRight = GetRawNoise2D((int) cellBotRight.x, (int) cellBotRight.y, seed);

		Vec2 botLeftToSample = sampleLocation - cell.mins;
		Vec2 topLeftToSample = sampleLocation - cellTopLeft;
		Vec2 topRightToSample = sampleLocation - cell.maxs;
		Vec2 botRightToSample = sampleLocation - cellBotRight;

		// The greater the distance to the left, the greater the weight of the right, and so on
		// Use bot left because its displacements are guaranteed to be between 0 and 1 (non negative)
		float weightRight = MathUtils::SmoothStep3(botLeftToSample.x);
		float weightTop = MathUtils::SmoothStep3(botLeftToSample.y);
		float weightLeft = 1.f - weightRight;
		float weightBot = 1.f - weightTop;

		// Pick a random facing vector from the list by using noise to randomly index into the gradient array
		int botLeftRandomIndex = noiseBotLeft & 0x0000'0007;
		int topLeftRandomIndex = noiseTopLeft & 0x0000'0007;
		int topRightRandomIndex = noiseTopRight & 0x0000'0007;
		int botRightRandomIndex = noiseBotRight & 0x0000'0007;

		Vec2 const& botLeftGradient = gradients[botLeftRandomIndex];
		Vec2 const& topLeftGradient = gradients[topLeftRandomIndex];
		Vec2 const& topRightGradient = gradients[topRightRandomIndex];
		Vec2 const& botRightGradient = gradients[botRightRandomIndex];

		float dotBotLeft = MathUtils::DotProduct2D(botLeftToSample, botLeftGradient);
		float dotTopLeft = MathUtils::DotProduct2D(topLeftToSample, topLeftGradient);
		float dotTopRight = MathUtils::DotProduct2D(topRightToSample, topRightGradient);
		float dotBotRight = MathUtils::DotProduct2D(botRightToSample, botRightGradient);

		float blendBot = (weightLeft * dotBotLeft) + (weightRight * dotBotRight);
		float blendTop = (weightLeft * dotTopLeft) + (weightRight* dotTopRight);
		float blendTotal = weightTop * blendTop + weightBot * blendBot;
		float noiseThisOctave = blendTotal * (1.f / 0.662578106f); // 2D Perlin is in [-.662578106,.662578106]; map to ~[-1,1]

		// Diminish by the octave persistence
		noiseThisOctave *= persistence;

		// Add persistence to the max amplitude - if all octaves were to add their max value at a single sample location, 
		// what would the value be there? (1 + 0.5 + 0.25 ...)
		maxAmplitude += persistence;

		sampleLocation *= octaveScale;
		sampleLocation += OCTAVE_OFFSET;

		totalNoise += noiseThisOctave;

		// Set data for next iteration
		persistence *= octavePersistence;
		++seed;
	}

	if (renormalize && maxAmplitude > 0)
	{
		totalNoise /= maxAmplitude; // Get back into range -1,1 from the range -A,A (where A = max amplitude from octaves)
		totalNoise = (totalNoise * 0.5f) + 0.5f;	// Map to 0,1 for smooth step
		totalNoise = MathUtils::SmoothStep3(totalNoise);		// Octaves tend to make noise middly, this pushes edges towards -1 and 1 again
		totalNoise = (totalNoise * 2.0f) - 1.f;		// Map back to -1,1
	}

	return totalNoise;
}



//----------------------------------------------------------------------------------------------------------------------
float GetPerlinNoise2D_01(float x, float y, float scale, unsigned int numOctaves, float octavePersistence, float octaveScale, bool renormalize, unsigned int seed)
{
	float noise = GetPerlinNoise2D(x, y, scale, numOctaves, octavePersistence, octaveScale, renormalize, seed);
	return noise * 0.5f + 0.5f;
}
