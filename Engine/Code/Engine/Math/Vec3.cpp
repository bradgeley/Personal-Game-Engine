// Bradley Christensen - 2022-2023
#include "Vec3.h"
#include "IntVec2.h"
#include "Vec2.h"
#include "MathUtils.h"



Vec3 Vec3::ZeroVector = Vec3(0.f, 0.f, 0.f);



Vec3::Vec3(float x, float y, float z) : x(x), y(y), z(z)
{
}


Vec3::Vec3(Vec2 const& fromVec2, float z) : x(fromVec2.x), y(fromVec2.y), z(z)
{
}


Vec3::Vec3(IntVec2 const& fromIntVec2, float z) : x(static_cast<float>(fromIntVec2.x)), y(static_cast<float>(fromIntVec2.y)), z(z)
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


float Vec3::GetLowest() const
{
    if (x <= y && x <= z)
    {
        return x;
    }
    if (y <= x && y <= z)
    {
        return y;
    }
    return z;
}


float Vec3::GetLowestXY() const
{
    if (x <= y)
    {
        return x;
    }
    return y;
}


float Vec3::GetHighest() const
{
    if (x >= y && x >= z)
    {
        return x;
    }
    if (y >= x && y >= z)
    {
        return y;
    }
    return z;
}


float Vec3::GetHighestXY() const
{
    if (x >= y)
    {
        return x;
    }
    return y;
}


Vec3 Vec3::operator-() const
{
    return Vec3(-x, -y, -z);
}


Vec3 Vec3::operator+(const Vec3& other) const
{
    return Vec3(x + other.x, y + other.y, z + other.z);
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


bool Vec3::operator==(Vec3 const& rhs) const
{
    return (x == rhs.x) && (y == rhs.y) && (z == rhs.z);
}
