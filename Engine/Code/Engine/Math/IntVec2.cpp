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


float IntVec2::GetAspect() const
{
    return (float) x / (float) y;
}



bool IntVec2::operator==(const IntVec2& other)
{
    return x == other.x && y == other.y;
}


IntVec2 IntVec2::operator-() const
{
    return IntVec2(-x, -y);
}


IntVec2 IntVec2::operator+(const IntVec2& other) const
{
    return IntVec2(x + other.x, y + other.y);
}


IntVec2 IntVec2::operator-(const IntVec2& other) const
{
    return IntVec2(x - other.x, y - other.y);
}


void IntVec2::operator+=(const IntVec2& other)
{
    x += other.x;
    y += other.y;
}


void IntVec2::operator-=(const IntVec2& other)
{
    x -= other.x;
    y -= other.y;
}
