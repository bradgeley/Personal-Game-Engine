// Bradley Christensen - 2022-2025
#pragma once



struct Vec2;



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
	explicit IntVec2(Vec2 const& fromVec2);

    bool IsZero() const;

    int GetLowest() const;
    int GetHighest() const;

    float GetAspect() const;
    IntVec2 GetCenter() const;

public:

    // Component-wise const operators
    bool operator==(IntVec2 const& other) const;
    bool operator!=(IntVec2 const& other) const;
    bool operator<(IntVec2 const& other) const;
    IntVec2 operator+(IntVec2 const& other) const;
    IntVec2 operator-(IntVec2 const& other) const;
    IntVec2 operator*(IntVec2 const& other) const;
    IntVec2 operator/(IntVec2 const& other) const;

    IntVec2 operator-() const;
    IntVec2 operator/(int divisor) const;
    IntVec2 operator*(int multiplier) const;
    IntVec2 operator*(float multiplier) const;


    // Component-wise self changing operators
    void operator+=(IntVec2 const& other);
    void operator-=(IntVec2 const& other);
    void operator*=(IntVec2 const& other);
    void operator/=(IntVec2 const& other);
    void operator*=(int multiplier);
    void operator/=(int divisor);

public:
    
    // Commonly used Vec2's
    static IntVec2 ZeroVector;
    static IntVec2 ZeroToOne;
};