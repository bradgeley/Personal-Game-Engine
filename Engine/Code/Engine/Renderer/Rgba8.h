// Bradley Christensen - 2022-2023
#pragma once
#include <cstdint>



struct Rgba8
{
public:
    
    uint8_t r = 255;
    uint8_t g = 255;
    uint8_t b = 255;
    uint8_t a = 255;
    
public:

    Rgba8() = default;
	explicit Rgba8(unsigned char r, unsigned char g, unsigned char b, unsigned char a = 255);

    void GetAsFloats(float* out_fourFloats) const;
    uint32_t GetAsUint32() const;

public:
    
    static Rgba8 Lerp(Rgba8 const& start, Rgba8 const& end, float t);
    
public:
    
    static Rgba8 const Red;
    static Rgba8 const Blue;
    static Rgba8 const LightBlue;
    static Rgba8 const Green;
    static Rgba8 const Cyan;
    static Rgba8 const DarkGreen;
    static Rgba8 const Black;
    static Rgba8 const LightGray;
    static Rgba8 const Gray;
    static Rgba8 const DarkGray;
    static Rgba8 const White;
    static Rgba8 const Gold;
    static Rgba8 const Orange;
    static Rgba8 const Yellow;
    static Rgba8 const Magenta;
    static Rgba8 const Empty;
};
