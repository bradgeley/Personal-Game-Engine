// Bradley Christensen - 2022-2023
#include "Vec2.h"
#include "IntVec2.h"
#include "Vec3.h"
#include "MathUtils.h"
#include "Engine/Core/ErrorUtils.h"



//----------------------------------------------------------------------------------------------------------------------
Vec2 Vec2::ZeroVector = Vec2(0.f, 0.f);
Vec2 Vec2::ZeroToOne = Vec2(0.f, 1.f);



//----------------------------------------------------------------------------------------------------------------------
Vec2::Vec2(float x, float y) : x(x), y(y)
{
}



//----------------------------------------------------------------------------------------------------------------------
Vec2::Vec2(int x, int y) : x(static_cast<float>(x)), y(static_cast<float>(y))
{
}



//----------------------------------------------------------------------------------------------------------------------
Vec2::Vec2(IntVec2 const& fromIntVec2) : x(static_cast<float>(fromIntVec2.x)), y(static_cast<float>(fromIntVec2.y))
{
}



//----------------------------------------------------------------------------------------------------------------------
Vec2::Vec2(Vec3 const& fromVec3) : x(fromVec3.x), y(fromVec3.y)
{
}



//----------------------------------------------------------------------------------------------------------------------
float Vec2::GetLength() const
{
    return MathUtils::GetLength2D(*this);
}



//----------------------------------------------------------------------------------------------------------------------
float Vec2::GetLengthSquared() const
{
    return MathUtils::GetLengthSquared2D(*this);
}



//----------------------------------------------------------------------------------------------------------------------
float Vec2::GetAngleDegrees() const
{
    return MathUtils::RadiansToDegrees(atan2f(y, x));
}



//----------------------------------------------------------------------------------------------------------------------
void Vec2::Normalize()
{
    MathUtils::NormalizeVector2D(*this);
}



//----------------------------------------------------------------------------------------------------------------------
Vec2 Vec2::GetNormalized() const
{
    Vec2 copy = *this;
    MathUtils::NormalizeVector2D(copy);
    return copy;
}



//----------------------------------------------------------------------------------------------------------------------
Vec2 Vec2::GetRotated90() const
{
    return Vec2(-y, x);
}



//----------------------------------------------------------------------------------------------------------------------
Vec2 Vec2::GetRotatedMinus90() const
{
    return Vec2(y, -x);
}



//----------------------------------------------------------------------------------------------------------------------
Vec2 Vec2::GetRotated(float degrees) const
{
    Vec2 result = *this;
    result.Rotate(degrees);
    return result;
}



//----------------------------------------------------------------------------------------------------------------------
Vec2 Vec2::GetProjectedOntoNormal(Vec2 const& normal) const
{
    return normal * MathUtils::DotProduct2D(*this, normal);
}



//----------------------------------------------------------------------------------------------------------------------
Vec2 Vec2::GetProjectedOnto(Vec2 const& vector) const
{
    float magnitudeInProjDir = MathUtils::DotProduct2D(*this, vector);
    return magnitudeInProjDir * vector / GetLengthSquared();

}



//----------------------------------------------------------------------------------------------------------------------
Vec2 Vec2::GetFloor() const
{
    Vec2 result = *this;
    result.Floor();
    return result;
}



//----------------------------------------------------------------------------------------------------------------------
float Vec2::GetDistanceTo(Vec2 const& other) const
{
    return MathUtils::GetDistance2D(*this, other);
}



//----------------------------------------------------------------------------------------------------------------------
float Vec2::GetDistanceSquaredTo(Vec2 const& other) const
{
    return MathUtils::GetDistanceSquared2D(*this, other);
}



//----------------------------------------------------------------------------------------------------------------------
bool Vec2::IsZero() const
{
    return (x == 0.f) && (y == 0.f);
}



//----------------------------------------------------------------------------------------------------------------------
bool Vec2::IsNearlyZero(float epsilon) const
{
    return (MathUtils::AbsF(x) < epsilon) && (MathUtils::AbsF(y) < epsilon);
}



//----------------------------------------------------------------------------------------------------------------------
float Vec2::Dot(Vec2 const& other) const
{
    return MathUtils::DotProduct2D(*this, other);
}



//----------------------------------------------------------------------------------------------------------------------
float Vec2::Cross(Vec2 const& other) const
{
    return MathUtils::CrossProduct2D(*this, other);
}



//----------------------------------------------------------------------------------------------------------------------
void Vec2::Rotate90()
{
    float X = x;
    x = -y;
    y = X;
}



//----------------------------------------------------------------------------------------------------------------------
void Vec2::RotateMinus90()
{
    float X = x;
    x = y;
    y = -X;
}



//----------------------------------------------------------------------------------------------------------------------
void Vec2::Rotate(float degrees)
{
    float angle = GetAngleDegrees();
    float length = GetLength();
    angle += degrees;
    x = MathUtils::CosDegrees(angle) * length;
    y = MathUtils::SinDegrees(angle) * length;
}



//----------------------------------------------------------------------------------------------------------------------
void Vec2::SetLength(float length)
{
    Normalize();
    *this *= length;
}



//----------------------------------------------------------------------------------------------------------------------
void Vec2::ClampLength(float maxLength)
{
    float length = GetLength();
    if (length > maxLength)
    {
        *this /= length; // Normalize
        *this *= maxLength; // Set length to new max
    }
}



//----------------------------------------------------------------------------------------------------------------------
void Vec2::ClampLengthMin(float minLength)
{
    float length = GetLength();
    if (length < minLength)
    {
        *this /= length; // Normalize
        *this *= minLength; // Set length to new min
    }
}



//----------------------------------------------------------------------------------------------------------------------
void Vec2::ClampLength(float minLength, float maxLength)
{
    float length = GetLength();
    if (length > maxLength)
    {
        *this /= length; // Normalize
        *this *= maxLength; // Set length to new max
    }
    else if (length < minLength)
    {
        *this /= length; // Normalize
        *this *= minLength; // Set length to new min
    }
}



//----------------------------------------------------------------------------------------------------------------------
void Vec2::Floor()
{
    x = static_cast<float>(MathUtils::FloorF(x));
    y = static_cast<float>(MathUtils::FloorF(y));
}



//----------------------------------------------------------------------------------------------------------------------
Vec2 Vec2::MakeFromUnitCircleDegrees(float angleDegrees)
{
    Vec2 result;
    result.x = MathUtils::CosDegrees(angleDegrees);
    result.y = MathUtils::SinDegrees(angleDegrees);
    return result;
}



//----------------------------------------------------------------------------------------------------------------------
Vec2 Vec2::MakeFromPolarCoords(float angleDegrees, float radius)
{
    return MakeFromUnitCircleDegrees(angleDegrees) * radius;
}



//----------------------------------------------------------------------------------------------------------------------
Vec2 Vec2::operator-() const
{
    return Vec2(-x, -y);
}



//----------------------------------------------------------------------------------------------------------------------
Vec2 Vec2::operator+(Vec2 const& other) const
{
    return Vec2(x + other.x, y + other.y);
}



//----------------------------------------------------------------------------------------------------------------------
Vec2 Vec2::operator-(Vec2 const& other) const
{
    return Vec2(x - other.x, y - other.y);
}



//----------------------------------------------------------------------------------------------------------------------
Vec2 Vec2::operator*(Vec2 const& other) const
{
    return Vec2(x * other.x, y * other.y);
}



//----------------------------------------------------------------------------------------------------------------------
Vec2 Vec2::operator/(Vec2 const& other) const
{
    return Vec2(x / other.x, y / other.y);
}



//----------------------------------------------------------------------------------------------------------------------
Vec2 Vec2::operator*(float multiplier) const
{
    return Vec2(x * multiplier, y * multiplier);
}



//----------------------------------------------------------------------------------------------------------------------
Vec2 Vec2::operator/(float divisor) const
{
    float oneOverDiv = 1.f / divisor;
    return Vec2(x * oneOverDiv, y * oneOverDiv);
}



//----------------------------------------------------------------------------------------------------------------------
void Vec2::operator+=(Vec2 const& other)
{
    x += other.x;
    y += other.y;
}



//----------------------------------------------------------------------------------------------------------------------
void Vec2::operator-=(Vec2 const& other)
{
    x -= other.x;
    y -= other.y;
}



//----------------------------------------------------------------------------------------------------------------------
void Vec2::operator*=(Vec2 const& other)
{
    x *= other.x;
    y *= other.y;
}



//----------------------------------------------------------------------------------------------------------------------
void Vec2::operator/=(Vec2 const& other)
{
    x /= other.x;
    y /= other.y;
}



//----------------------------------------------------------------------------------------------------------------------
void Vec2::operator*=(float multiplier)
{
    x *= multiplier;
    y *= multiplier;
}



//----------------------------------------------------------------------------------------------------------------------
void Vec2::operator/=(float divisor)
{
    float oneOverDiv = 1.f / divisor;
    x *= oneOverDiv;
    y *= oneOverDiv;
}



//----------------------------------------------------------------------------------------------------------------------
bool Vec2::operator==(Vec2 const& rhs) const
{
    return (x == rhs.x) && (y == rhs.y);
}



//----------------------------------------------------------------------------------------------------------------------
bool Vec2::operator!=(Vec2 const& rhs) const
{
    return (x != rhs.x) || (y != rhs.y);
}



//----------------------------------------------------------------------------------------------------------------------
Vec2 const operator*(float uniformScale, Vec2 const& vecToScale)
{
    return vecToScale * uniformScale;
}
