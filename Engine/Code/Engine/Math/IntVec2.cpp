// Bradley Christensen - 2022-2023
#include "Engine/Math/IntVec2.h"
#include "Engine/Math/Vec2.h"



IntVec2 IntVec2::ZeroVector = IntVec2(0, 0);
IntVec2 IntVec2::ZeroToOne = IntVec2(0, 1);



IntVec2::IntVec2() : x(0), y(0)
{
}


IntVec2::IntVec2(int x, int y) : x(x), y(y)
{
}


IntVec2::IntVec2(Vec2 const& fromVec2) : x(static_cast<int>(fromVec2.x)), y(static_cast<int>(fromVec2.y))
{
}


int IntVec2::GetLowest() const
{
    if (x <= y)
    {
        return x;
    }
    return y;
}


int IntVec2::GetHighest() const
{
    if (x >= y)
    {
        return x;
    }
    return y;
}


float IntVec2::GetAspect() const
{
    return (float) x / (float) y;
}


IntVec2 IntVec2::GetCenter() const
{
    return IntVec2(x / 2, y / 2);
}


bool IntVec2::operator==(IntVec2 const& other) const
{
    return x == other.x && y == other.y;
}


bool IntVec2::operator!=(IntVec2 const& other) const
{
    return x != other.x || y != other.y;
}


bool IntVec2::operator<(IntVec2 const& other) const
{
    if (y < other.y)
    {
        return true;
    }

    if (y > other.y)
    {
        return false;
    }

    return x < other.x;
}


IntVec2 IntVec2::operator-() const
{
    return IntVec2(-x, -y);
}


IntVec2 IntVec2::operator/(int divisor) const
{
    return IntVec2(x / divisor, y / divisor);
}


IntVec2 IntVec2::operator*(int divisor) const
{
    return IntVec2(x * divisor, y * divisor);
}


IntVec2 IntVec2::operator+(IntVec2 const& other) const
{
    return IntVec2(x + other.x, y + other.y);
}


IntVec2 IntVec2::operator-(IntVec2 const& other) const
{
    return IntVec2(x - other.x, y - other.y);
}


void IntVec2::operator+=(IntVec2 const& other)
{
    x += other.x;
    y += other.y;
}


void IntVec2::operator-=(IntVec2 const& other)
{
    x -= other.x;
    y -= other.y;
}
