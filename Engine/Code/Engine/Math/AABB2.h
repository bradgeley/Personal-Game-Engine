// Bradley Christensen - 2022-2025
#pragma once
#include "Vec2.h"



//----------------------------------------------------------------------------------------------------------------------
// AABB2
//
// Axis-aligned bounding box in 2d
//
struct AABB2
{
public:
    
    Vec2 mins = Vec2(0.f, 0.f);
    Vec2 maxs = Vec2(1.f, 1.f);

public:
    
    AABB2() = default;
    explicit AABB2(Vec2 const& mins, Vec2 const& maxs);
    explicit AABB2(float minX, float minY, float maxX, float maxY);
    explicit AABB2(Vec2 const& center, float halfWidth, float halfHeight);

    // Getters
    Vec2 GetCenter() const;
    Vec2 GetDimensions() const;
    Vec2 GetHalfDimensions() const;
    float GetWidth() const;
    float GetHeight() const;
    float GetHalfHeight() const;
    float GetAspect() const;
    Vec2 GetTopLeft() const;
    Vec2 GetBottomRight() const;
    Vec2 GetCenterLeft() const;
    Vec2 GetCenterRight() const;
    Vec2 GetTopCenter() const;
    Vec2 GetBottomCenter() const;
    AABB2 GetExpandedBy(float flatExpansionAmount) const;

    // Geometric Queries
    Vec2 GetNearestPoint(Vec2 const& point) const;
    bool IsPointInside(Vec2 const& point) const;
    bool IsOverlapping(AABB2 const& otherBox) const;

    // Mutators
    void Translate(Vec2 const& translation);
    void SetCenter(Vec2 const& newCenter);
    void SetDimsAboutCenter(Vec2 const& newDims);
    void SetHalfDimsAboutCenter(Vec2 const& newHalfDims);
    void Squeeze(float flatSqueezeAmount);
    void ExpandBy(float flatEpansionAmount);

public:
    
    static AABB2 const ZeroToOne;
};
