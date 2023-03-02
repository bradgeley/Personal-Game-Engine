// Bradley Christensen - 2022-2023
#pragma once



struct Vec3;



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
    
    Vec2() = default;
    explicit Vec2(float x, float y);
    explicit Vec2(int x, int y);
    explicit Vec2(Vec3 const& fromVec3);

public:
    
    float GetLength() const;
    void Normalize();
    Vec2 GetNormalized() const;
    Vec2 GetRotated90() const;
    Vec2 GetRotatedMinus90() const;
    
    void Rotate90();
    void RotateMinus90();

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
    
    bool operator==(Vec2 const& rhs) const;

public:
    
    // Commonly used Vec2's
    static Vec2 ZeroVector;
    static Vec2 ZeroToOne;
};
