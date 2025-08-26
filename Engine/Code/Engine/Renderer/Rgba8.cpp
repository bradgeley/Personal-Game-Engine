// Bradley Christensen - 2022-2025
#include "Engine/Renderer/Rgba8.h"

#include "Engine/Math/MathUtils.h"



// REDS (darkest to lightest)
Rgba8 const Rgba8::DarkRed(139, 0, 0);
Rgba8 const Rgba8::Maroon(128, 0, 0);
Rgba8 const Rgba8::Brown(165, 42, 42);
Rgba8 const Rgba8::Firebrick(178, 34, 34);
Rgba8 const Rgba8::Red(255, 0, 0);
Rgba8 const Rgba8::Crimson(220, 20, 60);
Rgba8 const Rgba8::Salmon(250, 128, 114);
Rgba8 const Rgba8::LightRed(255, 102, 102);
Rgba8 const Rgba8::Tomato(255, 99, 71);

// ORANGES (darkest to lightest)
Rgba8 const Rgba8::DarkOrange(255, 140, 0);
Rgba8 const Rgba8::Chocolate(210, 105, 30);
Rgba8 const Rgba8::Coral(255, 127, 80);
Rgba8 const Rgba8::Orange(255, 165, 0);
Rgba8 const Rgba8::Gold(255, 127, 0);
Rgba8 const Rgba8::LightOrange(255, 200, 124);

// YELLOWS (darkest to lightest)
Rgba8 const Rgba8::Goldenrod(218, 165, 32);
Rgba8 const Rgba8::DarkGoldenrod(184, 134, 11);
Rgba8 const Rgba8::Yellow(255, 255, 0);
Rgba8 const Rgba8::LightYellow(255, 255, 224);
Rgba8 const Rgba8::LemonChiffon(255, 250, 205);

// GREENS (darkest to lightest)
Rgba8 const Rgba8::DarkGreen(0, 127, 0);
Rgba8 const Rgba8::Olive(128, 128, 0);
Rgba8 const Rgba8::ForestGreen(34, 139, 34);
Rgba8 const Rgba8::SeaGreen(46, 139, 87);
Rgba8 const Rgba8::Green(0, 255, 0);
Rgba8 const Rgba8::PastelGreen(119, 221, 119);
Rgba8 const Rgba8::Lime(0, 255, 0);
Rgba8 const Rgba8::LightGreen(144, 238, 144);
Rgba8 const Rgba8::Honeydew(240, 255, 240);

// BLUES (darkest to lightest)
Rgba8 const Rgba8::DarkBlue(0, 0, 150);
Rgba8 const Rgba8::Navy(0, 0, 128);
Rgba8 const Rgba8::DarkOceanBlue(6, 66, 115);
Rgba8 const Rgba8::MediumBlue(0, 0, 205);
Rgba8 const Rgba8::Blue(0, 0, 255);
Rgba8 const Rgba8::Cerulean(0, 171, 240);
Rgba8 const Rgba8::LightOceanBlue(29, 162, 216);
Rgba8 const Rgba8::SkyBlue(135, 206, 235);
Rgba8 const Rgba8::LightBlue(51, 255, 255);
Rgba8 const Rgba8::Cyan(0, 255, 255);
Rgba8 const Rgba8::AliceBlue(240, 248, 255);

// INDIGOS (darkest to lightest)
Rgba8 const Rgba8::DarkIndigo(46, 0, 77);
Rgba8 const Rgba8::Indigo(75, 0, 130);
Rgba8 const Rgba8::SlateBlue(106, 90, 205);
Rgba8 const Rgba8::MediumSlateBlue(123, 104, 238);

// VIOLETS (darkest to lightest)
Rgba8 const Rgba8::Purple(128, 0, 128);
Rgba8 const Rgba8::DarkViolet(148, 0, 211);
Rgba8 const Rgba8::Orchid(218, 112, 214);
Rgba8 const Rgba8::Magenta(255, 0, 255);
Rgba8 const Rgba8::Violet(238, 130, 238);
Rgba8 const Rgba8::Lavender(230, 230, 250);

// GRAYS & NEUTRALS (darkest to lightest)
Rgba8 const Rgba8::Black(0, 0, 0);
Rgba8 const Rgba8::DarkGray(50, 50, 50);
Rgba8 const Rgba8::Gray(100, 100, 100, 255);
Rgba8 const Rgba8::LightGray(150, 150, 150);
Rgba8 const Rgba8::Gainsboro(220, 220, 220);
Rgba8 const Rgba8::White(255, 255, 255);
Rgba8 const Rgba8::TransparentBlack(0, 0, 0, 0);
Rgba8 const Rgba8::TransparentWhite(255, 255, 255, 0);
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
bool Rgba8::operator==(Rgba8 const& other) const
{
    return r == other.r && g == other.g && b == other.b && a == other.a;
}



//----------------------------------------------------------------------------------------------------------------------
bool Rgba8::operator!=(Rgba8 const& other) const
{
    return r != other.r || g != other.g || b != other.b || a != other.a;
}



//----------------------------------------------------------------------------------------------------------------------
Rgba8 Rgba8::Lerp(Rgba8 const& start, Rgba8 const& end, float t)
{
    Rgba8 result;
    result.r = static_cast<uint8_t>(MathUtils::InterpolateInt(start.r, end.r, t));
    result.g = static_cast<uint8_t>(MathUtils::InterpolateInt(start.g, end.g, t));
    result.b = static_cast<uint8_t>(MathUtils::InterpolateInt(start.b, end.b, t));
    result.a = static_cast<uint8_t>(MathUtils::InterpolateInt(start.a, end.a, t));
    return result;
}



//----------------------------------------------------------------------------------------------------------------------
Rgba8 Rgba8::Blend(Rgba8 const& a, Rgba8 const& b)
{
    Rgba8 result;
    result.r = (uint8_t) (float)((a.r + b.r) * 0.5f);
    result.g = (uint8_t) (float)((a.g + b.g) * 0.5f);
    result.b = (uint8_t) (float)((a.b + b.b) * 0.5f);
    result.a = (uint8_t) (float)((a.a + b.a) * 0.5f);
    return result;
}