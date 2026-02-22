// Bradley Christensen - 2022-2026
#include "GeometryUtils.h"
#include "MathUtils.h"
#include "Constants.h"
#include "AABB2.h"
#include "Plane2.h"
#include "Engine/Core/ErrorUtils.h"
#include <cfloat>



//----------------------------------------------------------------------------------------------------------------------
bool GeometryUtils::IsPointInsideDisc2D(Vec2 const& point, Vec2 const& discCenter, float discRadius)
{
    float distSquared = (point - discCenter).GetLengthSquared();
    if (distSquared > discRadius * discRadius)
    {
        return false;
    }
    return true;
}



//----------------------------------------------------------------------------------------------------------------------
bool GeometryUtils::DoDiscsOverlap2D(Vec2 const& position1, float radius1, Vec2 const& position2, float radius2)
{
    float distanceSquared = MathUtils::GetDistanceSquared2D(position1, position2);
    float combinedRadii = (radius1 + radius2);
    float combinedRadiiSquared = combinedRadii * combinedRadii;
    if (distanceSquared >= combinedRadiiSquared)
    {
        return false;
    }

    return true;
}



//----------------------------------------------------------------------------------------------------------------------
bool GeometryUtils::DoesDiscOverlapAABB(Vec2 const& discPos, float discRadius, AABB2 const& aabb)
{
    Vec2 nearestPointOnAABB = aabb.GetNearestPoint(discPos);
    return IsPointInsideDisc2D(nearestPointOnAABB, discPos, discRadius);
}



//----------------------------------------------------------------------------------------------------------------------
bool GeometryUtils::DoesRingOverlapAABB(Vec2 const& ringPos, float innerRadius, float outerRadius, AABB2 const& aabb)
{
    Vec2 nearestPointOnAABB = aabb.GetNearestPoint(ringPos);
    bool isWithinOuterRadius = IsPointInsideDisc2D(nearestPointOnAABB, ringPos, outerRadius);
    if (!isWithinOuterRadius)
    {
        return false;
	}

	Vec2 farthestPointOnAABB = aabb.GetFarthestPoint(ringPos);
	bool isFullyWithinInnerRadius = IsPointInsideDisc2D(farthestPointOnAABB, ringPos, innerRadius);
    if (isFullyWithinInnerRadius)
    {
        return false;
	}

	return true;
}



//----------------------------------------------------------------------------------------------------------------------
bool GeometryUtils::DoesCapsuleOverlapAABB(Vec2 const& capsuleStart, Vec2 const& capsuleEnd, float capsuleRadius, AABB2 const& aabb)
{
    AABB2 capsuleBounds = GetCapsuleBounds(capsuleStart, capsuleEnd, capsuleRadius);

    if (!capsuleBounds.IsOverlapping(aabb))
    {
        return false;
    }

    float combinedRadii = capsuleRadius + aabb.GetHeight() * MathUtils::SQRT2OVER2F;
    float distSquaredToLine = GetShortestDistanceSquaredToLineSegment(aabb.GetCenter(), capsuleStart, capsuleEnd);
    if (distSquaredToLine > combinedRadii * combinedRadii)
    {
        return false;
    }

    if (aabb.IsPointInside(capsuleStart) || aabb.IsPointInside(capsuleEnd))
    {
        return true;
    }

    float distanceSquared = GetShortestDistanceSquaredBetweenLineSegmentAndAABB(capsuleStart, capsuleEnd, aabb);
    if (distanceSquared > (capsuleRadius * capsuleRadius))
    {
        return false;
    }

    return true;
}



//----------------------------------------------------------------------------------------------------------------------
bool GeometryUtils::PushDiscOutOfPoint2D(Vec2& discPos, float radius, Vec2 const& point)
{
    Vec2 pointToDisc = (discPos - point);
    float distSquared = pointToDisc.GetLengthSquared();
    if (distSquared == 0.f)
    {
        // Can't determine which direction to push
        return false;
    }

    if (distSquared < (radius * radius))
    {
        float distance = MathUtils::SqrtF(distSquared);
        Vec2 pointToDiscNormal = pointToDisc / distance;
        discPos = point + pointToDiscNormal * radius;
        return true;
    }
    return false;
}



//----------------------------------------------------------------------------------------------------------------------
bool GeometryUtils::PushDiscOutOfDisc2D(Vec2& mobileDiscPos, float mobileDiscRadius, Vec2 const& staticDiscPos, float staticDiscRadius)
{
    float combinedRadii = (mobileDiscRadius + staticDiscRadius);
    return PushDiscOutOfPoint2D(mobileDiscPos, combinedRadii, staticDiscPos);
}



//----------------------------------------------------------------------------------------------------------------------
bool GeometryUtils::PushDiscsOutOfEachOther2D(Vec2& discPosA, float discRadiusA, Vec2& discPosB, float discRadiusB, bool weightByRadius /*= true*/, float dampening /*= 1.f*/)
{
    if (discPosA == discPosB)
    {
        return false;
    }

    Vec2 AtoB = discPosB - discPosA;
    float distanceSquared = AtoB.GetLengthSquared();

    float combinedRadii = (discRadiusA + discRadiusB);
    float combinedRadiiSquared = combinedRadii * combinedRadii;
    if (distanceSquared < combinedRadiiSquared)
    {
        float distance = MathUtils::SqrtF(distanceSquared);
        float overlapAmount = (combinedRadii - distance) * dampening;

        AtoB /= distance; // Normalize

		float weightA, weightB;
        if (weightByRadius)
        {
            weightA = discRadiusB / combinedRadii;
            weightB = 1.f - weightA;
        }
        else
        {
			weightA = 0.5f;
			weightB = 0.5f;
        }

        Vec2 displacementA = AtoB * -1.f * overlapAmount * weightA;
        Vec2 displacementB = AtoB * overlapAmount * weightB;

        discPosA += displacementA;
        discPosB += displacementB;

        return true;
    }
    return false;
}



//----------------------------------------------------------------------------------------------------------------------
bool GeometryUtils::PushDiscOutOfAABB2D(Vec2& discPos, float discRadius, AABB2 const& aabb)
{
    Vec2 nearestPointOnAABB = aabb.GetNearestPoint(discPos);
    if (!IsPointInsideDisc2D(nearestPointOnAABB, discPos, discRadius))
    {
        return false;
    }

    PushDiscOutOfPoint2D(discPos, discRadius, nearestPointOnAABB);
    return true;
}



//----------------------------------------------------------------------------------------------------------------------
AABB2 GeometryUtils::GetDiscBounds(Vec2 const& center, float radius)
{
    return AABB2(center - Vec2(radius, radius), center + Vec2(radius, radius));
}



//----------------------------------------------------------------------------------------------------------------------
AABB2 GeometryUtils::GetCapsuleBounds(Vec2 const& start, Vec2 const& end, float radius)
{
    float minX = MathUtils::Min(start.x - radius, end.x - radius);
    float minY = MathUtils::Min(start.y - radius, end.y - radius);
    float maxX = MathUtils::Max(start.x + radius, end.x + radius);
    float maxY = MathUtils::Max(start.y + radius, end.y + radius);
    return AABB2(minX, minY, maxX, maxY);
}



//----------------------------------------------------------------------------------------------------------------------
bool GeometryUtils::BounceDiscsOffEachOther2D(Vec2& discPosA, float discRadiusA, Vec2& discVelA, Vec2& discPosB, float discRadiusB, Vec2& discVelB, float elasticity)
{
    Vec2 AtoB = discPosB - discPosA;
    float distanceSquared = AtoB.GetLengthSquared();
    float combinedRadii = (discRadiusA + discRadiusB);
    float combinedRadiiSquared = combinedRadii * combinedRadii;
    if (distanceSquared < combinedRadiiSquared)
    {
        // Push discs out of each other first
        float distance = MathUtils::SqrtF(distanceSquared);
        float overlapAmount = combinedRadii - distance;

        AtoB /= distance; // Normalize

        float weightA = discRadiusB / combinedRadii;
        float weightB = 1.f - weightA;

        Vec2 displacementA = AtoB * -1.f * overlapAmount * weightA;
        Vec2 displacementB = AtoB * overlapAmount * weightB;

        discPosA += displacementA;
        discPosB += displacementB;

        // Put everything in terms of Va = 0
        Vec2 relativeVelA = Vec2::ZeroVector;
        Vec2 relativeVelB = discVelB - discVelA;

        // Bounce B off of A
        float dotB = MathUtils::DotProduct2D(relativeVelB, AtoB);
        if (dotB > 0.f)
        {
            // B's velocity dot's positive with AToB, so they are moving apart and we shouldn't bounce them
            return false;
        }

        Vec2 projectedVelB = AtoB * dotB;
        Vec2 remainingVelB = relativeVelB - projectedVelB;

        Vec2 velocityTransfer = projectedVelB * elasticity;

        relativeVelB -= velocityTransfer;
        relativeVelA += velocityTransfer;

        // Add back what was lost before
        discVelB = relativeVelB + discVelA;
        discVelA = relativeVelA + discVelA;

        return true;
    }
    return false;
}



//----------------------------------------------------------------------------------------------------------------------
bool GeometryUtils::BounceDiscsOffEachOther2D(Vec2& discPosA, float discRadiusA, Vec2& discVelA, float massA, Vec2& discPosB, float discRadiusB, Vec2& discVelB, float massB, float elasticity)
{
    Vec2 AtoB = discPosB - discPosA;
    float distanceSquared = AtoB.GetLengthSquared();
    float combinedRadii = (discRadiusA + discRadiusB);
    float combinedRadiiSquared = combinedRadii * combinedRadii;
    if (distanceSquared < combinedRadiiSquared)
    {
        // Push discs out of each other first (no prediction yet)
        float distance = MathUtils::SqrtF(distanceSquared);
        float overlapAmount = combinedRadii - distance;

        AtoB /= distance; // Normalize
        Vec2& collisionNormal = AtoB;

        // Use mass to determine who pushes who out the most
        float oneOverCombinedMass = 1.f / (massA + massB);
        float weightA = massB * oneOverCombinedMass;
        float weightB = 1.f - weightA;

        Vec2 displacementA = AtoB * -1.f * overlapAmount * weightA;
        Vec2 displacementB = AtoB * overlapAmount * weightB;

        discPosA += displacementA;
        discPosB += displacementB;

        float A_speedNorm = MathUtils::DotProduct2D(discVelA, collisionNormal);
        float B_speedNorm = MathUtils::DotProduct2D(discVelB, collisionNormal);
        float speedDiffInitial = A_speedNorm - B_speedNorm;
        if (speedDiffInitial > 0.f)
        {
            Vec2 velA_i = A_speedNorm * collisionNormal;
            Vec2 velB_i = B_speedNorm * collisionNormal;
            Vec2 velA_j = discVelA - velA_i;
            Vec2 velB_j = discVelB - velB_i;

            float halfSpeedDiffFinal = (0.5f) * elasticity * speedDiffInitial;
            float finalVelocity = (massA * A_speedNorm + massB * B_speedNorm) * oneOverCombinedMass;
            float A_speedNormFinal = finalVelocity - halfSpeedDiffFinal;
            float B_speedNormFinal = finalVelocity + halfSpeedDiffFinal;

            discVelA = A_speedNormFinal * collisionNormal + velA_j;
            discVelB = B_speedNormFinal * collisionNormal + velB_j;

            return true;
        }
    }
    return false;
}



//----------------------------------------------------------------------------------------------------------------------
bool GeometryUtils::DoLinesIntersect(Vec2 const& a1, Vec2 const& a2, Vec2 const& b1, Vec2 const& b2)
{
    Vec2 lineA = a2 - a1;
    if ((lineA.Cross(b1 - a1) * lineA.Cross(b2 - a1)) >= 0.f)
    {
        return false;
    }

    Vec2 lineB = b2 - b1;
    if ((lineB.Cross(a1 - b1) * lineB.Cross(a2 - b1)) >= 0.f)
    {
        return false;
    }

    return true;
}



//----------------------------------------------------------------------------------------------------------------------
bool GeometryUtils::DoesLineIntersectAABB2(Vec2 const& lineStart, Vec2 const& lineEnd, AABB2 const& box2D)
{
    AABB2 lineBounds = AABB2(lineStart, lineEnd);

    if (!lineBounds.IsOverlapping(box2D))
    {
        return false;
    }

    if (box2D.IsPointInside(lineStart) || box2D.IsPointInside(lineEnd))
    {
        return true;
    }

    Vec2 topLeft = box2D.GetTopLeft();
    Vec2 botRight = box2D.GetBottomRight();
    Vec2 const& topRight = box2D.maxs;
    Vec2 const& botLeft = box2D.mins;

    Vec2 line = lineEnd - lineStart;

    return DoLinesIntersect(topLeft, topRight, lineStart, lineEnd) 
        || DoLinesIntersect(topRight, botRight, lineStart, lineEnd) 
        || DoLinesIntersect(botRight, botLeft, lineStart, lineEnd) 
        || DoLinesIntersect(botLeft, topLeft, lineStart, lineEnd);
}



//----------------------------------------------------------------------------------------------------------------------
bool GeometryUtils::GetFirstLineAABBIntersection(Vec2 const& lineStart, Vec2 const& lineEnd, AABB2 const& box2D, float& out_tOfFirstIntersection)
{
    Vec2 velocity = lineEnd - lineStart;
    Vec2 oneOverVelocity = Vec2(1.f / velocity.x, 1.f / velocity.y);

    float resultEntryT = -std::numeric_limits<float>::infinity();
    float resultExitT = std::numeric_limits<float>::infinity();

    constexpr float axisAlignedTolerance = 0.000001f;

    // Check x direction first
    if (MathUtils::AbsF(velocity.x) < axisAlignedTolerance)
    {
        if (lineStart.x < box2D.mins.x || lineStart.x > box2D.maxs.x)
        {
            // X is a separating axis
            return false;
        }
    }
    else
    {
        float tEnterX = (box2D.mins.x - lineStart.x) * oneOverVelocity.x;
        float tExitX = (box2D.maxs.x - lineStart.x) * oneOverVelocity.x;

        resultEntryT = MathUtils::Max(resultEntryT, MathUtils::Min(tEnterX, tExitX));
        resultExitT = MathUtils::Min(resultExitT, MathUtils::Max(tEnterX, tExitX));
    }

    // then Y
    if (MathUtils::AbsF(velocity.y) < axisAlignedTolerance)
    {
        if (lineStart.y < box2D.mins.y || lineStart.y > box2D.maxs.y)
        {
            // Y is a separating axis
            return false;
        }
    }
    else
    {
        float tEnterY = (box2D.mins.y - lineStart.y) * oneOverVelocity.y;
        float tExitY = (box2D.maxs.y - lineStart.y) * oneOverVelocity.y;

        resultEntryT = MathUtils::Max(resultEntryT, MathUtils::Min(tEnterY, tExitY));
        resultExitT = MathUtils::Min(resultExitT, MathUtils::Max(tEnterY, tExitY));
    }

    if (resultExitT >= resultEntryT && resultExitT >= 0.0f)
    {
        out_tOfFirstIntersection = resultEntryT >= 0.0f ? resultEntryT : resultExitT;
        return true;
    }

    return false;
}



//----------------------------------------------------------------------------------------------------------------------
Vec2 GeometryUtils::GetNearestPointOnLine(Vec2 const& queryPoint, Vec2 const& lineStart, Vec2 const& lineEnd)
{
    Vec2 line = lineEnd - lineStart;
    if (line.IsNearlyZero(0.000001f))
    {
        return lineStart;
    }

    Vec2 startToPoint = queryPoint - lineStart;
    float squaredLength = line.GetLengthSquared();
    float dot = line.Dot(startToPoint);
    float fractionAlongSegment = dot / squaredLength;

    // Check if query point is beyond either end point of the line
    if (fractionAlongSegment >= 1.f)
    {
        return lineEnd;
    }
    else if (fractionAlongSegment <= 0.f)
    {
        return lineStart;
    }

    // Point is between the end points, relatively speaking
    return lineStart + line * fractionAlongSegment;
}



//----------------------------------------------------------------------------------------------------------------------
float GeometryUtils::GetShortestDistanceToLineSegment(Vec2 const& queryPoint, Vec2 const& lineStart, Vec2 const& lineEnd)
{
    Vec2 nearestPoint = GetNearestPointOnLine(queryPoint, lineStart, lineEnd);
    return queryPoint.GetDistanceTo(nearestPoint);
}



//----------------------------------------------------------------------------------------------------------------------
float GeometryUtils::GetShortestDistanceSquaredToLineSegment(Vec2 const& queryPoint, Vec2 const& lineStart, Vec2 const& lineEnd)
{
    Vec2 nearestPoint = GetNearestPointOnLine(queryPoint, lineStart, lineEnd);
    return queryPoint.GetDistanceSquaredTo(nearestPoint);
}



//----------------------------------------------------------------------------------------------------------------------
float GeometryUtils::GetShortestDistanceBetweenLineSegments(Vec2 const& aStart, Vec2 const& aEnd, Vec2 const& bStart, Vec2 const& bEnd)
{
    return MathUtils::SqrtF(GetShortestDistanceSquaredBetweenLineSegments(aStart, aEnd, bStart, bEnd));
}



//----------------------------------------------------------------------------------------------------------------------
float GeometryUtils::GetShortestDistanceSquaredBetweenLineSegments(Vec2 const& aStart, Vec2 const& aEnd, Vec2 const& bStart, Vec2 const& bEnd)
{
    if (aStart == aEnd)
    {
        return GeometryUtils::GetShortestDistanceSquaredToLineSegment(aStart, bStart, bEnd);
    }
    if (bStart == bEnd)
    {
        return GeometryUtils::GetShortestDistanceSquaredToLineSegment(bStart, aStart, bEnd);
    }

    if (DoLinesIntersect(aStart, aEnd, bStart, bEnd))
    {
        return 0.f;
    }

    Vec2 nearestToAStart = GetNearestPointOnLine(aStart, bStart, bEnd);
    Vec2 nearestToAEnd = GetNearestPointOnLine(aEnd, bStart, bEnd);
    Vec2 nearestToBStart = GetNearestPointOnLine(bStart, aStart, aEnd);
    Vec2 nearestToBEnd = GetNearestPointOnLine(bEnd, aStart, aEnd);

    float distSquaredToAStart = nearestToAStart.GetDistanceSquaredTo(aStart);
    float distSquaredToAEnd = nearestToAEnd.GetDistanceSquaredTo(aEnd);
    float distSquaredToBStart = nearestToBStart.GetDistanceSquaredTo(bStart);
    float distSquaredToBEnd = nearestToBEnd.GetDistanceSquaredTo(bEnd);

    float shortestDistanceSquared = FLT_MAX;
    shortestDistanceSquared = MathUtils::Min(shortestDistanceSquared, distSquaredToAStart);
    shortestDistanceSquared = MathUtils::Min(shortestDistanceSquared, distSquaredToAEnd);
    shortestDistanceSquared = MathUtils::Min(shortestDistanceSquared, distSquaredToBStart);
    shortestDistanceSquared = MathUtils::Min(shortestDistanceSquared, distSquaredToBEnd);

    return shortestDistanceSquared;
}



//----------------------------------------------------------------------------------------------------------------------
float GeometryUtils::GetShortestDistanceBetweenLineSegmentAndAABB(Vec2 const& lineStart, Vec2 const& lineEnd, AABB2 const& aabb)
{
    return MathUtils::SqrtF(GetShortestDistanceSquaredBetweenLineSegmentAndAABB(lineStart, lineEnd, aabb));
}



//----------------------------------------------------------------------------------------------------------------------
float GeometryUtils::GetShortestDistanceSquaredBetweenLineSegmentAndAABB(Vec2 const& lineStart, Vec2 const& lineEnd, AABB2 const& aabb)
{
    // Check all 4 lines of the AABB with the query line
    float distSquaredToTop = GetShortestDistanceSquaredBetweenLineSegments(lineStart, lineEnd, aabb.GetTopLeft(), aabb.maxs);
    float distSquaredToRight = GetShortestDistanceSquaredBetweenLineSegments(lineStart, lineEnd, aabb.maxs, aabb.GetBottomRight());
    float distSquaredToLeft = GetShortestDistanceSquaredBetweenLineSegments(lineStart, lineEnd, aabb.GetTopLeft(), aabb.mins);
    float distSquaredToBottom = GetShortestDistanceSquaredBetweenLineSegments(lineStart, lineEnd, aabb.mins, aabb.GetBottomRight());

    // Separating Axis Theorem    
    float shortestDistanceSquared = FLT_MAX;
    shortestDistanceSquared = MathUtils::Min(shortestDistanceSquared, distSquaredToTop);
    shortestDistanceSquared = MathUtils::Min(shortestDistanceSquared, distSquaredToRight);
    shortestDistanceSquared = MathUtils::Min(shortestDistanceSquared, distSquaredToLeft);
    shortestDistanceSquared = MathUtils::Min(shortestDistanceSquared, distSquaredToBottom);

    return shortestDistanceSquared;
}



//----------------------------------------------------------------------------------------------------------------------
bool GeometryUtils::SweepDiscVsPoint(Vec2 const& discStart, Vec2 const& discEnd, float discRadius, Vec2 const& point, float& out_tOfIntersection)
{
    Vec2 velocity = discEnd - discStart;
    Vec2 delta = discStart - point;
    float root1, root2;

    float a = velocity.x * velocity.x + velocity.y * velocity.y;
    float b = 2.f * (delta.x * velocity.x + delta.y * velocity.y);
    float c = (delta.x * delta.x) + (delta.y * delta.y) - (discRadius * discRadius);

    int numRoots = MathUtils::QuadraticEquation(a, b, c, root1, root2);

    float t;
    if (numRoots == 1)
    {
        t = root1;
    }
    else if (numRoots == 2)
    {
        t = MathUtils::Min(root1, root2);
    }
    else
    {
        return false;
    }

    if (t >= 0.f && t <= 1.f)
    {
        out_tOfIntersection = t;
        return true;
    }

    return false;
}
