// Bradley Christensen - 2022-2023
#include "IntVec2.h"



IntVec2 IntVec2::ZeroVector = IntVec2(0, 0);
IntVec2 IntVec2::ZeroToOne = IntVec2(0, 1);



IntVec2::IntVec2() : x(0), y(0)
{
}


IntVec2::IntVec2(int x, int y) : x(x), y(y)
{
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
