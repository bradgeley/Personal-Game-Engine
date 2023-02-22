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

public:
    
    static Rgba8 const WHITE;
};
