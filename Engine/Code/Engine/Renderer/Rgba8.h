// Bradley Christensen - 2022-2025
#pragma once
#include <cstdint>



//----------------------------------------------------------------------------------------------------------------------
struct Rgba8
{
public:
    
    uint8_t r = 255;
    uint8_t g = 255;
    uint8_t b = 255;
    uint8_t a = 255;
    
public:

    Rgba8() = default;
	Rgba8(unsigned char r, unsigned char g, unsigned char b, unsigned char a = 255);

    void GetAsFloats(float* out_fourFloats) const;
    uint32_t GetAsUint32() const;

    bool operator==(Rgba8 const& other) const;
    bool operator!=(Rgba8 const& other) const;

	Rgba8 operator*(float scale) const;

public:
    
    static Rgba8 Lerp(Rgba8 const& start, Rgba8 const& end, float t);
    static Rgba8 Blend(Rgba8 const& a, Rgba8 const& b);
    
public:
    
    // REDS (darkest to lightest)
    static Rgba8 const DarkRed;
    static Rgba8 const Maroon;
    static Rgba8 const Brown;
    static Rgba8 const Firebrick;
    static Rgba8 const Red;
    static Rgba8 const Crimson;
    static Rgba8 const Salmon;
    static Rgba8 const LightRed;
    static Rgba8 const Tomato;

    // ORANGES (darkest to lightest)
    static Rgba8 const DarkOrange;
    static Rgba8 const Chocolate;
    static Rgba8 const Coral;
    static Rgba8 const Orange;
    static Rgba8 const Gold;
    static Rgba8 const LightOrange;

    // YELLOWS (darkest to lightest)
    static Rgba8 const Goldenrod;
    static Rgba8 const DarkGoldenrod;
    static Rgba8 const Yellow;
    static Rgba8 const LightYellow;
    static Rgba8 const LemonChiffon;

    // GREENS (darkest to lightest)
    static Rgba8 const DarkGreen;
    static Rgba8 const Olive;
    static Rgba8 const ForestGreen;
    static Rgba8 const SeaGreen;
    static Rgba8 const Green;
    static Rgba8 const PastelGreen;
    static Rgba8 const Lime;
    static Rgba8 const LightGreen;
    static Rgba8 const Honeydew;

    // BLUES (darkest to lightest)
    static Rgba8 const DarkBlue;
    static Rgba8 const Navy;
    static Rgba8 const DarkOceanBlue;
    static Rgba8 const MediumBlue;
    static Rgba8 const Blue;
    static Rgba8 const Cerulean;
    static Rgba8 const LightOceanBlue;
    static Rgba8 const SkyBlue;
    static Rgba8 const LightBlue;
    static Rgba8 const Cyan;
    static Rgba8 const AliceBlue;

    // INDIGOS (darkest to lightest)
    static Rgba8 const DarkIndigo;
    static Rgba8 const Indigo;
    static Rgba8 const SlateBlue;
    static Rgba8 const MediumSlateBlue;

    // VIOLETS (darkest to lightest)
    static Rgba8 const Purple;
    static Rgba8 const DarkViolet;
    static Rgba8 const Orchid;
    static Rgba8 const Magenta;
    static Rgba8 const Violet;
    static Rgba8 const Lavender;

    // GRAYS & NEUTRALS (darkest to lightest)
    static Rgba8 const Black;
    static Rgba8 const DarkGray;
    static Rgba8 const Gray;
    static Rgba8 const LightGray;
    static Rgba8 const Gainsboro;
    static Rgba8 const White;
    static Rgba8 const TransparentBlack;
    static Rgba8 const TransparentWhite;
    static Rgba8 const Empty;
};
