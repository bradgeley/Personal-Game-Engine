// Bradley Christensen - 2022-2025
#include "Vec3.h"
#include "IntVec2.h"
#include "Vec2.h"
#include "MathUtils.h"



//----------------------------------------------------------------------------------------------------------------------
Vec3 Vec3::ZeroVector = Vec3(0.f, 0.f, 0.f);
Vec3 Vec3::OneVector = Vec3(1.f, 1.f, 1.f);



//----------------------------------------------------------------------------------------------------------------------
Vec3::Vec3(float x, float y, float z) : x(x), y(y), z(z)
{
}



//----------------------------------------------------------------------------------------------------------------------
Vec3::Vec3(Vec2 const& fromVec2, float z) : x(fromVec2.x), y(fromVec2.y), z(z)
{
}



//----------------------------------------------------------------------------------------------------------------------
Vec3::Vec3(IntVec2 const& fromIntVec2, float z) : x(static_cast<float>(fromIntVec2.x)), y(static_cast<float>(fromIntVec2.y)), z(z)
{
}



//----------------------------------------------------------------------------------------------------------------------
float Vec3::GetLength() const
{
    return MathUtils::GetLength3D(*this);
}



//----------------------------------------------------------------------------------------------------------------------
float Vec3::GetLengthSquared() const
{
    return MathUtils::GetLengthSquared3D(*this);
}



//----------------------------------------------------------------------------------------------------------------------
float Vec3::GetDistanceTo(Vec3 const& other) const
{
    return MathUtils::GetDistance3D(*this, other);
}



//----------------------------------------------------------------------------------------------------------------------
float Vec3::GetDistanceSquaredTo(Vec3 const& other) const
{
    return MathUtils::GetDistanceSquared3D(*this, other);
}



//----------------------------------------------------------------------------------------------------------------------
void Vec3::Normalize()
{
    MathUtils::NormalizeVector3D(*this);
}



//----------------------------------------------------------------------------------------------------------------------
Vec3 Vec3::GetNormalized() const
{
    Vec3 copy = *this;
    MathUtils::NormalizeVector3D(copy);
    return copy;
}



//----------------------------------------------------------------------------------------------------------------------
float Vec3::Dot(Vec3 const& other) const
{
    return x * other.x + y * other.y + z * other.z;
}



//----------------------------------------------------------------------------------------------------------------------
Vec3 Vec3::Cross(Vec3 const& other) const
{
    return Vec3(y * other.z - z * other.y, z * other.x - x * other.z, x * other.y - y * other.x);
}



//----------------------------------------------------------------------------------------------------------------------
bool Vec3::IsZero() const
{
    return (x == 0.f) && (y == 0.f) && (z == 0.f);
}



//----------------------------------------------------------------------------------------------------------------------
bool Vec3::IsNearlyZero(float epsilon) const
{
    return (MathUtils::AbsF(x) < epsilon) && (MathUtils::AbsF(y) < epsilon) && (MathUtils::AbsF(z) < epsilon);
}



//----------------------------------------------------------------------------------------------------------------------
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



//----------------------------------------------------------------------------------------------------------------------
float Vec3::GetLowestXY() const
{
    if (x <= y)
    {
        return x;
    }
    return y;
}



//----------------------------------------------------------------------------------------------------------------------
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



//----------------------------------------------------------------------------------------------------------------------
float Vec3::GetHighestXY() const
{
    if (x >= y)
    {
        return x;
    }
    return y;
}



//----------------------------------------------------------------------------------------------------------------------
Vec3 Vec3::operator-() const
{
    return Vec3(-x, -y, -z);
}



//----------------------------------------------------------------------------------------------------------------------
Vec3 Vec3::operator+(Vec3 const& other) const
{
    return Vec3(x + other.x, y + other.y, z + other.z);
}



//----------------------------------------------------------------------------------------------------------------------
Vec3 Vec3::operator-(Vec3 const& other) const
{
    return Vec3(x - other.x, y - other.y, z - other.z);
}



//----------------------------------------------------------------------------------------------------------------------
Vec3 Vec3::operator*(Vec3 const& other) const
{
    return Vec3(x * other.x, y * other.y, z * other.z);
}



//----------------------------------------------------------------------------------------------------------------------
Vec3 Vec3::operator/(Vec3 const& other) const
{
    return Vec3(x / other.x, y / other.y, z / other.z);
}



//----------------------------------------------------------------------------------------------------------------------
Vec3 Vec3::operator*(float multiplier) const
{
    return Vec3(x * multiplier, y * multiplier, z * multiplier);
}



//----------------------------------------------------------------------------------------------------------------------
Vec3 Vec3::operator/(float divisor) const
{
    float oneOverDiv = 1.f / divisor;
    return Vec3(x * oneOverDiv, y * oneOverDiv, z * oneOverDiv);
}



//----------------------------------------------------------------------------------------------------------------------
void Vec3::operator+=(Vec3 const& other)
{                     
    x += other.x;     
    y += other.y;     
    z += other.z;
}



//----------------------------------------------------------------------------------------------------------------------
void Vec3::operator-=(Vec3 const& other)
{
    x -= other.x;
    y -= other.y;
    z -= other.z;
}



//----------------------------------------------------------------------------------------------------------------------
void Vec3::operator*=(Vec3 const& other)
{
    x *= other.x;
    y *= other.y;
    z *= other.z;
}



//----------------------------------------------------------------------------------------------------------------------
void Vec3::operator/=(Vec3 const& other)
{
    x /= other.x;
    y /= other.y;
    z /= other.z;
}



//----------------------------------------------------------------------------------------------------------------------
void Vec3::operator*=(float multiplier)
{
    x *= multiplier;
    y *= multiplier;
    z *= multiplier;
}



//----------------------------------------------------------------------------------------------------------------------
void Vec3::operator/=(float divisor)
{
    float oneOverDiv = 1.f / divisor;
    x *= oneOverDiv;
    y *= oneOverDiv;
    z *= oneOverDiv;
}



//----------------------------------------------------------------------------------------------------------------------
bool Vec3::operator==(Vec3 const& rhs) const
{
    return (x == rhs.x) && (y == rhs.y) && (z == rhs.z);
}



//----------------------------------------------------------------------------------------------------------------------
bool Vec3::operator!=(Vec3 const& rhs) const
{
    return (x != rhs.x) || (y != rhs.y) || (z != rhs.z);
}
