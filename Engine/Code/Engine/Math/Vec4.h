// Bradley Christensen - 2022-2026
#pragma once



struct Vec3;



//----------------------------------------------------------------------------------------------------------------------
// Vec4
//
struct Vec4
{
public:

    float x = 0.f;
    float y = 0.f;
    float z = 0.f;
    float w = 0.f;

public:
    
    Vec4() = default;
    explicit Vec4(float x, float y, float z, float w = 0.f);
    explicit Vec4(Vec3 const& fromVec3, float w = 0.f);

public:

    // Component-wise const operators
    Vec4 operator-() const;

    Vec4 operator+(Vec4 const& other) const;
    Vec4 operator-(Vec4 const& other) const;
    Vec4 operator*(Vec4 const& other) const;
    Vec4 operator/(Vec4 const& other) const;

    Vec4 operator*(float multiplier) const;
    Vec4 operator/(float divisor) const;

    // Component-wise self changing operators
    void operator+=(Vec4 const& other);
    void operator-=(Vec4 const& other);
    void operator*=(Vec4 const& other);
    void operator/=(Vec4 const& other);

    void operator*=(float multiplier);
    void operator/=(float divisor);
    
    bool operator==(Vec4 const& rhs) const;
    bool operator!=(Vec4 const& rhs) const;

public:
    
    // Commonly used Vec4's
    static Vec4 ZeroVector;
    static Vec4 OneVector;
};