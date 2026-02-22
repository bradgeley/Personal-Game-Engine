// Bradley Christensen - 2022-2026
#include "Engine/Math/Plane2.h"
#include "Engine/Core/ErrorUtils.h"
#include "MathUtils.h"



//----------------------------------------------------------------------------------------------------------------------
Plane2::Plane2(Vec2 const& normal, float distance) : m_normal(normal), m_distance(distance)
{
    ASSERT_OR_DIE(normal.IsNormalized(), "Plane2 constructor normal was not normalized.");
}



//----------------------------------------------------------------------------------------------------------------------
Plane2::Plane2(Vec2 const& from, Vec2 const& to)
{
    Vec2 edge = (to - from);
    m_normal = edge.GetRotated90().GetNormalized();
    m_distance = MathUtils::DotProduct2D(from, m_normal);
}



//----------------------------------------------------------------------------------------------------------------------
float Plane2::GetAltitude(Vec2 const& point) const
{
    float d = MathUtils::DotProduct2D(point, m_normal);
    return d - m_distance;
}



//----------------------------------------------------------------------------------------------------------------------
float Plane2::GetDistance(Vec2 const& point) const
{
    return MathUtils::AbsF(GetAltitude(point));
}



//----------------------------------------------------------------------------------------------------------------------
bool Plane2::Straddles(Vec2 const& a, Vec2 const& b) const
{
	return GetAltitude(a) * GetAltitude(b) < 0.f;
}



//----------------------------------------------------------------------------------------------------------------------
bool Plane2::operator==(Plane2 const& rhs) const 
{
    constexpr float epsilon = 0.000001f;
    return MathUtils::Abs(m_distance - rhs.m_distance) < epsilon 
           && m_normal.IsNearlyEqual(rhs.m_normal, epsilon);
}



//----------------------------------------------------------------------------------------------------------------------
bool Plane2::operator!=(Plane2 const& rhs) const 
{
    constexpr float epsilon = 0.000001f;
    return !MathUtils::IsNearlyEqual(m_distance, rhs.m_distance, epsilon)
        || !m_normal.IsNearlyEqual(rhs.m_normal, epsilon);
}