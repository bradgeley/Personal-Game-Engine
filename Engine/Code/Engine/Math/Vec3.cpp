﻿// Bradley Christensen - 2022
#include "Vec3.h"
#include "MathUtils.h"


Vec3 Vec3::ZeroVector = Vec3(0.f, 0.f, 0.f);


Vec3::Vec3() : x(0.f), y(0.f)
{
}


Vec3::Vec3(float x, float y, float z) : x(x), y(y), z(z)
{
}


float Vec3::GetLength() const
{
    return GetLength3D(*this);
}


void Vec3::Normalize()
{
    NormalizeVector3D(*this);
}


Vec3 Vec3::GetNormalized() const
{
    Vec3 copy = *this;
    NormalizeVector3D(copy);
    return copy;
}


Vec3 Vec3::operator-() const
{
    return Vec3(-x, -y, -z);
}


Vec3 Vec3::operator+(const Vec3& other) const
{
    return Vec3(x + other.x, y + other.y, y + other.z);
}


Vec3 Vec3::operator-(const Vec3& other) const
{
    return Vec3(x - other.x, y - other.y, z - other.z);
}


Vec3 Vec3::operator*(const Vec3& other) const
{
    return Vec3(x * other.x, y * other.y, z * other.z);
}


Vec3 Vec3::operator/(const Vec3& other) const
{
    return Vec3(x / other.x, y / other.y, z / other.z);
}


Vec3 Vec3::operator*(float multiplier) const
{
    return Vec3(x * multiplier, y * multiplier, z * multiplier);
}


Vec3 Vec3::operator/(float divisor) const
{
    float oneOverDiv = 1.f / divisor;
    return Vec3(x * oneOverDiv, y * oneOverDiv, z * oneOverDiv);
}


void Vec3::operator+=(const Vec3& other)
{
    x += other.x;
    y += other.y;
    z += other.z;
}


void Vec3::operator-=(const Vec3& other)
{
    x -= other.x;
    y -= other.y;
    z -= other.z;
}


void Vec3::operator*=(const Vec3& other)
{
    x *= other.x;
    y *= other.y;
    z *= other.z;
}


void Vec3::operator/=(const Vec3& other)
{
    x /= other.x;
    y /= other.y;
    z /= other.z;
}


void Vec3::operator*=(float multiplier)
{
    x *= multiplier;
    y *= multiplier;
    z *= multiplier;
}


void Vec3::operator/=(float divisor)
{
    float oneOverDiv = 1.f / divisor;
    x *= oneOverDiv;
    y *= oneOverDiv;
    z *= oneOverDiv;
}
