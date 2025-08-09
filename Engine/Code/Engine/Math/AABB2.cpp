﻿// Bradley Christensen - 2022-2023
#include "Engine/Math/AABB2.h"
#include "MathUtils.h"



AABB2 const AABB2::ZeroToOne = AABB2();



AABB2::AABB2(Vec2 const& mins, Vec2 const& maxs) :
    mins(MinF(mins.x, maxs.x), MinF(mins.y, maxs.y)),
    maxs(MaxF(mins.x, maxs.x), MaxF(mins.y, maxs.y))
{
}



AABB2::AABB2(float minX, float minY, float maxX, float maxY) :
    mins(MinF(minX, maxX), MinF(minY, maxY)),
    maxs(MaxF(minX, maxX), MaxF(minY, maxY))
{
}



AABB2::AABB2(Vec2 const& center, float halfWidth, float halfHeight)
    : mins(center - Vec2(halfWidth, halfHeight)), maxs(center + Vec2(halfWidth, halfHeight))
{
}



Vec2 AABB2::GetCenter() const
{
    return mins + GetHalfDimensions();
}



Vec2 AABB2::GetDimensions() const
{
    return maxs - mins;
}



Vec2 AABB2::GetHalfDimensions() const
{
    return (maxs - mins) * 0.5f;
}



float AABB2::GetWidth() const
{
    return maxs.x - mins.x;
}



float AABB2::GetHeight() const
{
    return maxs.y - mins.y;
}



float AABB2::GetAspect() const
{
    return GetWidth() / GetHeight();
}



Vec2 AABB2::GetTopLeft() const
{
    return Vec2(mins.x, maxs.y);
}



Vec2 AABB2::GetBottomRight() const
{
    return Vec2(maxs.x, mins.y);
}



Vec2 AABB2::GetCenterLeft() const
{
    return Vec2(mins.x, (maxs.y + mins.y) * 0.5f);
}



Vec2 AABB2::GetCenterRight() const
{
    return Vec2(maxs.x, (maxs.y + mins.y) * 0.5f);
}



Vec2 AABB2::GetNearestPoint(Vec2 const& point) const
{
    Vec2 result = point;
    result.x = ClampF(result.x, mins.x, maxs.x);
    result.y = ClampF(result.y, mins.y, maxs.y);
    return result;
}



bool AABB2::IsPointInside(Vec2 const& point) const
{
    return (point.x > mins.x) &&
           (point.y > mins.y) &&
           (point.x < maxs.x) &&
           (point.y < maxs.y);
}



bool AABB2::IsOverlapping(AABB2 const& otherBox) const
{
    if ((otherBox.mins.x >= mins.x && otherBox.mins.x <= maxs.x) ||
        (otherBox.maxs.x >= mins.x && otherBox.maxs.x <= maxs.x) ||
        (otherBox.mins.x >= mins.x && otherBox.maxs.x <= maxs.x) ||
        (otherBox.mins.x <= mins.x && otherBox.maxs.x >= maxs.x))
    {
        if ((otherBox.mins.y >= mins.y && otherBox.mins.y <= maxs.y) ||
            (otherBox.maxs.y >= mins.y && otherBox.maxs.y <= maxs.y) ||
            (otherBox.mins.y >= mins.y && otherBox.maxs.y <= maxs.y) ||
            (otherBox.mins.y <= mins.y && otherBox.maxs.y >= maxs.y))
        {
            return true;
        }
    }
    return false;
}



void AABB2::Translate(Vec2 const& translation)
{
    mins += translation;
    maxs += translation;
}



void AABB2::SetCenter(Vec2 const& newCenter)
{
    Vec2 halfDims = GetHalfDimensions();
    mins = newCenter - halfDims;
    maxs = newCenter + halfDims;
}



void AABB2::SetDimsAboutCenter(Vec2 const& newDims)
{
    Vec2 center = GetCenter();
    Vec2 newHalfDims = newDims * 0.5f;
    mins = center - newHalfDims;
    maxs = center + newHalfDims;
}



void AABB2::SetHalfDimsAboutCenter(Vec2 const& newHalfDims)
{
    Vec2 center = GetCenter();
    mins = center - newHalfDims;
    maxs = center + newHalfDims;
}



void AABB2::Squeeze(float squeezeAmount)
{
    Vec2 newMins = mins + Vec2(squeezeAmount, squeezeAmount);
    Vec2 newMaxs = maxs - Vec2(squeezeAmount, squeezeAmount);
    if (newMins.x > newMaxs.x || newMins.y > newMaxs.y)
    {
        Vec2 avg = (mins + maxs) / 2;
        mins = avg;
        maxs = avg;
    }
    else
    {
        mins = newMins;
        maxs = newMaxs;
    }
}
