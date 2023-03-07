// Bradley Christensen - 2022-2023
#include "Vec2.h"
#include "IntVec2.h"
#include "Vec3.h"
#include "MathUtils.h"



Vec2 Vec2::ZeroVector = Vec2(0.f, 0.f);
Vec2 Vec2::ZeroToOne = Vec2(0.f, 1.f);



Vec2::Vec2(float x, float y) : x(x), y(y)
{
}


Vec2::Vec2(int x, int y) : x(static_cast<float>(x)), y(static_cast<float>(y))
{
}


Vec2::Vec2(IntVec2 const& fromIntVec2) : x(static_cast<float>(fromIntVec2.x)), y(static_cast<float>(fromIntVec2.y))
{
}


Vec2::Vec2(Vec3 const& fromVec3) : x(fromVec3.x), y(fromVec3.y)
{
}


float Vec2::GetLength() const
{
    return GetLength2D(*this);
}


void Vec2::Normalize()
{
    NormalizeVector2D(*this);
}


Vec2 Vec2::GetNormalized() const
{
    Vec2 copy = *this;
    NormalizeVector2D(copy);
    return copy;
}


Vec2 Vec2::GetRotated90() const
{
    return Vec2(-y, x);
}


Vec2 Vec2::GetRotatedMinus90() const
{
    return Vec2(y, -x);
}


void Vec2::Rotate90()
{
    float X = x;
    x = -y;
    y = X;
}


void Vec2::RotateMinus90()
{
    float X = x;
    x = y;
    y = -X;
}


Vec2 Vec2::operator-() const
{
    return Vec2(-x, -y);
}


Vec2 Vec2::operator+(const Vec2& other) const
{
    return Vec2(x + other.x, y + other.y);
}


Vec2 Vec2::operator-(const Vec2& other) const
{
    return Vec2(x - other.x, y - other.y);
}


Vec2 Vec2::operator*(const Vec2& other) const
{
    return Vec2(x * other.x, y * other.y);
}


Vec2 Vec2::operator/(const Vec2& other) const
{
    return Vec2(x / other.x, y / other.y);
}


Vec2 Vec2::operator*(float multiplier) const
{
    return Vec2(x * multiplier, y * multiplier);
}


Vec2 Vec2::operator/(float divisor) const
{
    float oneOverDiv = 1.f / divisor;
    return Vec2(x * oneOverDiv, y * oneOverDiv);
}


void Vec2::operator+=(const Vec2& other)
{
    x += other.x;
    y += other.y;
}


void Vec2::operator-=(const Vec2& other)
{
    x -= other.x;
    y -= other.y;
}


void Vec2::operator*=(const Vec2& other)
{
    x *= other.x;
    y *= other.y;
}


void Vec2::operator/=(const Vec2& other)
{
    x /= other.x;
    y /= other.y;
}


void Vec2::operator*=(float multiplier)
{
    x *= multiplier;
    y *= multiplier;
}


void Vec2::operator/=(float divisor)
{
    float oneOverDiv = 1.f / divisor;
    x *= oneOverDiv;
    y *= oneOverDiv;
}


bool Vec2::operator==(Vec2 const& rhs) const
{
    return (x == rhs.x) && (y == rhs.y);
}
