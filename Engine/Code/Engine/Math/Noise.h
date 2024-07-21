// Bradley Christensen - 2022-2023
#pragma once



//----------------------------------------------------------------------------------------------------------------------
// Noise Functions
//
// Credit: Squirrel Eiserloh
//

//----------------------------------------------------------------------------------------------------------------------
// Raw Noise
// 
// Returns uint between 0 and MAX_UINT
//
unsigned int GetRawNoise1D(int x, int seed);
unsigned int GetRawNoise2D(int x, int y, int seed);

//----------------------------------------------------------------------------------------------------------------------
// Noise
// 
// Returns float between 0 and 1
//
float GetNoiseZeroToOne1D(int x, int seed);
float GetNoiseZeroToOne2D(int x, int y, int seed);

//----------------------------------------------------------------------------------------------------------------------
// Fractal Noise
// 
// Returns float between -1 and 1
//
float GetFractalNoise1D(float position, float scale = 1.f, unsigned int numOctaves = 1, float octavePersistence = 0.5f, float octaveScale = 2.f, bool renormalize = true, unsigned int seed = 0);
float GetFractalNoise2D(float x, float y, float scale = 1.f, unsigned int numOctaves = 1, float octavePersistence = 0.5f, float octaveScale = 2.f, bool renormalize = true, unsigned int seed = 0);

//----------------------------------------------------------------------------------------------------------------------
// Perlin Noise
// 
// Returns float between -1 and 1, smoother than fractal noise
//
float GetPerlinNoise1D(float position, float scale = 1.f, unsigned int numOctaves = 1, float octavePersistence = 0.5f, float octaveScale = 2.f, bool renormalize = true, unsigned int seed = 0);
float GetPerlinNoise2D(float x, float y, float scale = 1.f, unsigned int numOctaves = 1, float octavePersistence = 0.5f, float octaveScale = 2.f, bool renormalize = true, unsigned int seed = 0);
