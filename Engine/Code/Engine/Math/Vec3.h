﻿// Bradley Christensen - 2022-2025
#pragma once



struct Vec2;
struct IntVec2;



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
    explicit Vec3(float x, float y, float z = 0.f);
    explicit Vec3(Vec2 const& fromVec2, float z = 0.f);
    explicit Vec3(IntVec2 const& fromIntVec2, float z = 0.f);

public:

    void Normalize();

    float GetLength() const;
	float GetLengthSquared() const;
	float GetDistanceTo(Vec3 const& other) const;
	float GetDistanceSquaredTo(Vec3 const& other) const;
    Vec3 GetNormalized() const;
	float Dot(Vec3 const& other) const;
    Vec3 Cross(Vec3 const& other) const;

    bool IsZero() const;
	bool IsNearlyZero(float epsilon = 0.000001f) const;
    float GetLowest() const;
    float GetLowestXY() const;
    float GetHighest() const;
    float GetHighestXY() const;

public:

    // Component-wise const operators
    Vec3 operator-() const;

    Vec3 operator+(Vec3 const& other) const;
    Vec3 operator-(Vec3 const& other) const;
    Vec3 operator*(Vec3 const& other) const;
    Vec3 operator/(Vec3 const& other) const;

    Vec3 operator*(float multiplier) const;
    Vec3 operator/(float divisor) const;

    // Component-wise self changing operators
    void operator+=(Vec3 const& other);
    void operator-=(Vec3 const& other);
    void operator*=(Vec3 const& other);
    void operator/=(Vec3 const& other);

    void operator*=(float multiplier);
    void operator/=(float divisor);
    
    bool operator==(Vec3 const& rhs) const;
    bool operator!=(Vec3 const& rhs) const;

public:
    
    // Commonly used Vec3's
    static Vec3 ZeroVector;
    static Vec3 OneVector;
};
