// Bradley Christensen - 2022-2026
#include "Vec4.h"
#include "Vec3.h"



//----------------------------------------------------------------------------------------------------------------------
Vec4 Vec4::ZeroVector = Vec4(0.f, 0.f, 0.f, 0.f);
Vec4 Vec4::OneVector = Vec4(1.f, 1.f, 1.f, 1.f);



//----------------------------------------------------------------------------------------------------------------------
Vec4::Vec4(float x, float y, float z, float w) : x(x), y(y), z(z), w(w)
{
}



//----------------------------------------------------------------------------------------------------------------------
Vec4::Vec4(Vec3 const& fromVec3, float w) : x(fromVec3.x), y(fromVec3.y), z(fromVec3.z), w(w)
{
}



//----------------------------------------------------------------------------------------------------------------------
Vec4 Vec4::operator-() const
{
    return Vec4(-x, -y, -z, -w);
}



//----------------------------------------------------------------------------------------------------------------------
Vec4 Vec4::operator+(Vec4 const& other) const
{
    return Vec4(x + other.x, y + other.y, z + other.z, w + other.w);
}



//----------------------------------------------------------------------------------------------------------------------
Vec4 Vec4::operator-(Vec4 const& other) const
{
    return Vec4(x - other.x, y - other.y, z - other.z, w - other.w);
}



//----------------------------------------------------------------------------------------------------------------------
Vec4 Vec4::operator*(Vec4 const& other) const
{
    return Vec4(x * other.x, y * other.y, z * other.z, w * other.w);
}



//----------------------------------------------------------------------------------------------------------------------
Vec4 Vec4::operator/(Vec4 const& other) const
{
    return Vec4(x / other.x, y / other.y, z / other.z, w / other.w);
}



//----------------------------------------------------------------------------------------------------------------------
Vec4 Vec4::operator*(float multiplier) const
{
    return Vec4(x * multiplier, y * multiplier, z * multiplier, w * multiplier);
}



//----------------------------------------------------------------------------------------------------------------------
Vec4 Vec4::operator/(float divisor) const
{
    float oneOverDiv = 1.f / divisor;
    return Vec4(x * oneOverDiv, y * oneOverDiv, z * oneOverDiv, w * oneOverDiv);
}



//----------------------------------------------------------------------------------------------------------------------
void Vec4::operator+=(Vec4 const& other)
{
    x += other.x;
    y += other.y;
    z += other.z;
    w += other.w;
}



//----------------------------------------------------------------------------------------------------------------------
void Vec4::operator-=(Vec4 const& other)
{
    x -= other.x;
    y -= other.y;
    z -= other.z;
    w -= other.w;
}



//----------------------------------------------------------------------------------------------------------------------
void Vec4::operator*=(Vec4 const& other)
{
    x *= other.x;
    y *= other.y;
    z *= other.z;
    w *= other.w;
}



//----------------------------------------------------------------------------------------------------------------------
void Vec4::operator/=(Vec4 const& other)
{
    x /= other.x;
    y /= other.y;
    z /= other.z;
    w /= other.w;
}



//----------------------------------------------------------------------------------------------------------------------
void Vec4::operator*=(float multiplier)
{
    x *= multiplier;
    y *= multiplier;
    z *= multiplier;
    w *= multiplier;
}



//----------------------------------------------------------------------------------------------------------------------
void Vec4::operator/=(float divisor)
{
    float oneOverDiv = 1.f / divisor;
    x *= oneOverDiv;
    y *= oneOverDiv;
    z *= oneOverDiv;
    w *= oneOverDiv;
}



//----------------------------------------------------------------------------------------------------------------------
bool Vec4::operator==(Vec4 const& rhs) const
{
    return (x == rhs.x) && (y == rhs.y) && (z == rhs.z) && (w == rhs.w);
}



//----------------------------------------------------------------------------------------------------------------------
bool Vec4::operator!=(Vec4 const& rhs) const
{
    return (x != rhs.x) || (y != rhs.y) || (z != rhs.z) || (w != rhs.w);
}