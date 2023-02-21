// Bradley Christensen - 2022
#pragma once



//----------------------------------------------------------------------------------------------------------------------
// IntVec2
//
// A couple of ints
//
struct IntVec2
{
public:
    
    int x = 0;
    int y = 0;

public:
    
    explicit IntVec2();
    explicit IntVec2(int x, int y);

public:

    // Component-wise const operators
    IntVec2 operator-() const;

    IntVec2 operator+(const IntVec2& other) const;
    IntVec2 operator-(const IntVec2& other) const;

    // Component-wise self changing operators
    void operator+=(const IntVec2& other);
    void operator-=(const IntVec2& other);

public:
    
    // Commonly used Vec2's
    static IntVec2 ZeroVector;
    static IntVec2 ZeroToOne;
};
