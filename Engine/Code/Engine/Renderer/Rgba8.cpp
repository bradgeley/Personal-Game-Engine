// Bradley Christensen - 2022-2023
#include "Engine/Renderer/Rgba8.h"

#include "Engine/Math/MathUtils.h"



//----------------------------------------------------------------------------------------------------------------------
Rgba8 const Rgba8::Red(255, 0, 0);
Rgba8 const Rgba8::Blue(0, 0, 255);
Rgba8 const Rgba8::LightBlue(51,255,255);
Rgba8 const Rgba8::Cerulean(0,171,240);
Rgba8 const Rgba8::Green(0, 255, 0);
Rgba8 const Rgba8::Cyan(0, 255, 255);
Rgba8 const Rgba8::DarkGreen(0, 127, 0);
Rgba8 const Rgba8::Black(0, 0, 0);
Rgba8 const Rgba8::LightGray(150, 150, 150);
Rgba8 const Rgba8::DarkGray(50, 50, 50);
Rgba8 const Rgba8::Gray(100, 100, 100, 255);
Rgba8 const Rgba8::White(255, 255, 255);
Rgba8 const Rgba8::Gold(255, 127, 0);
Rgba8 const Rgba8::Yellow(255, 255, 0);
Rgba8 const Rgba8::Orange(255, 165, 0);
Rgba8 const Rgba8::Magenta(255, 0, 255);
Rgba8 const Rgba8::Empty(0, 0, 0, 0);



//----------------------------------------------------------------------------------------------------------------------
Rgba8::Rgba8(unsigned char r, unsigned char g, unsigned char b, unsigned char a)
    : r(r), g(g), b(b), a(a)
{
}



//----------------------------------------------------------------------------------------------------------------------
void Rgba8::GetAsFloats(float* out_fourFloats) const
{
    out_fourFloats[0] = static_cast<float>(r) / 255.f;
    out_fourFloats[1] = static_cast<float>(g) / 255.f;
    out_fourFloats[2] = static_cast<float>(b) / 255.f;
    out_fourFloats[3] = static_cast<float>(a) / 255.f;
}



//----------------------------------------------------------------------------------------------------------------------
uint32_t Rgba8::GetAsUint32() const
{
    void* addressOfR = (void*) &r;
    uint32_t* asInt = reinterpret_cast<uint32_t*>(addressOfR);
    uint32_t copy = *asInt;
    return copy;
}



//----------------------------------------------------------------------------------------------------------------------
Rgba8 Rgba8::Lerp(Rgba8 const& start, Rgba8 const& end, float t)
{
    Rgba8 result;
    result.r = static_cast<uint8_t>(InterpolateInt(start.r, end.r, t));
    result.g = static_cast<uint8_t>(InterpolateInt(start.g, end.g, t));
    result.b = static_cast<uint8_t>(InterpolateInt(start.b, end.b, t));
    result.a = static_cast<uint8_t>(InterpolateInt(start.a, end.a, t));
    return result;
}
