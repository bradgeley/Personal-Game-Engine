// Bradley Christensen - 2022-2023
#pragma once



struct Vec2;



//----------------------------------------------------------------------------------------------------------------------
// Vec3
//
// A couple of floats, representing a position or change of position in 2D space.
//
struct Vec3
{
public:

    float x = 0.f;
    float y = 0.f;
    float z = 0.f;

public:
    
    Vec3() = default;
    explicit Vec3(float x, float y, float z);
    explicit Vec3(Vec2 const& fromVec2);

public:

    float GetLength() const;
    void Normalize();
    Vec3 GetNormalized() const;

    // Component-wise const operators
    Vec3 operator-() const;

    Vec3 operator+(const Vec3& other) const;
    Vec3 operator-(const Vec3& other) const;
    Vec3 operator*(const Vec3& other) const;
    Vec3 operator/(const Vec3& other) const;

    Vec3 operator*(float multiplier) const;
    Vec3 operator/(float divisor) const;

    // Component-wise self changing operators
    void operator+=(const Vec3& other);
    void operator-=(const Vec3& other);
    void operator*=(const Vec3& other);
    void operator/=(const Vec3& other);

    void operator*=(float multiplier);
    void operator/=(float divisor);
    
    bool operator==(Vec3 const& rhs) const;

public:
    
    // Commonly used Vec3's
    static Vec3 ZeroVector;
};
