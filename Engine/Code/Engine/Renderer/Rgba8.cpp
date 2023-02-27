﻿// Bradley Christensen - 2022-2023
#include "Engine/Renderer/Rgba8.h"

#include "Engine/Math/MathUtils.h"


Rgba8 const Rgba8::RED(255, 0, 0);
Rgba8 const Rgba8::BLUE(0, 0, 255);
Rgba8 const Rgba8::LIGHT_BLUE(51,255,255);
Rgba8 const Rgba8::GREEN(0, 255, 0);
Rgba8 const Rgba8::CYAN(0, 255, 255);
Rgba8 const Rgba8::DARK_GREEN(0, 127, 0);
Rgba8 const Rgba8::BLACK(0, 0, 0);
Rgba8 const Rgba8::LIGHT_GRAY(150, 150, 150);
Rgba8 const Rgba8::DARK_GRAY(50, 50, 50);
Rgba8 const Rgba8::GRAY(100, 100, 100, 255);
Rgba8 const Rgba8::WHITE(255, 255, 255);
Rgba8 const Rgba8::GOLD(255, 127, 0);
Rgba8 const Rgba8::YELLOW(255, 255, 0);
Rgba8 const Rgba8::ORANGE(255, 165, 0);
Rgba8 const Rgba8::MAGENTA(255, 0, 255);
Rgba8 const Rgba8::EMPTY(0, 0, 0, 0);



Rgba8::Rgba8(unsigned char r, unsigned char g, unsigned char b, unsigned char a)
    : r(r), g(g), b(b), a(a)
{
}



void Rgba8::GetAsFloats(float* fourFloatOutputArray) const
{
    fourFloatOutputArray[0] = static_cast<float>(r) / 255.f;
    fourFloatOutputArray[1] = static_cast<float>(g) / 255.f;
    fourFloatOutputArray[2] = static_cast<float>(b) / 255.f;
    fourFloatOutputArray[3] = static_cast<float>(a) / 255.f;
}



uint32_t Rgba8::GetAsUint32() const
{
    void* addressOfR = (void*) &r;
    int* asInt = reinterpret_cast<int*>(addressOfR);
    int copy = *asInt;
    return copy;
}



Rgba8 Rgba8::Lerp(Rgba8 const& start, Rgba8 const& end, float t)
{
    Rgba8 result;
    result.r = static_cast<uint8_t>(InterpolateInt(start.r, end.r, t));
    result.g = static_cast<uint8_t>(InterpolateInt(start.g, end.g, t));
    result.b = static_cast<uint8_t>(InterpolateInt(start.b, end.b, t));
    result.a = static_cast<uint8_t>(InterpolateInt(start.a, end.a, t));
    return result;
}
