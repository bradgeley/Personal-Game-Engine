// Bradley Christensen - 2022
#pragma once


//----------------------------------------------------------------------------------------------------------------------
// Vec2
//
// A couple of floats, representing a position or change of position in 2D space.
//
struct Vec2
{
public:
    float x = 0.f;
    float y = 0.f;

public:
    explicit Vec2();
    explicit Vec2(float x, float y);

public:
    float GetLength() const;
    void Normalize();
    Vec2 GetNormalized() const;

    // Component-wise const operators
    Vec2 operator-() const;

    Vec2 operator+(const Vec2& other) const;
    Vec2 operator-(const Vec2& other) const;
    Vec2 operator*(const Vec2& other) const;
    Vec2 operator/(const Vec2& other) const;

    Vec2 operator*(float multiplier) const;
    Vec2 operator/(float divisor) const;

    // Component-wise self changing operators
    void operator+=(const Vec2& other);
    void operator-=(const Vec2& other);
    void operator*=(const Vec2& other);
    void operator/=(const Vec2& other);

    void operator*=(float multiplier);
    void operator/=(float divisor);

public:
    static Vec2 ZeroVector;
    static Vec2 ZeroToOne;
};
