// Bradley Christensen - 2022
#pragma once
#include <cstdint>



struct Rgba8
{
public:
    
    uint8_t r;
    uint8_t g;
    uint8_t b;
    uint8_t a;
    
public:
    
	explicit Rgba8(unsigned char r, unsigned char g, unsigned char b, unsigned char a = 255);

    void GetAsFloats(float* fourFloatOutputArray) const;
    
public:
    
    static Rgba8 const RED;
    static Rgba8 const BLUE;
    static Rgba8 const LIGHT_BLUE;
    static Rgba8 const GREEN;
    static Rgba8 const CYAN;
    static Rgba8 const DARK_GREEN;
    static Rgba8 const BLACK;
    static Rgba8 const LIGHT_GRAY;
    static Rgba8 const GRAY;
    static Rgba8 const DARK_GRAY;
    static Rgba8 const WHITE;
    static Rgba8 const GOLD;
    static Rgba8 const ORANGE;
    static Rgba8 const YELLOW;
    static Rgba8 const MAGENTA;
    static Rgba8 const EMPTY;
};
