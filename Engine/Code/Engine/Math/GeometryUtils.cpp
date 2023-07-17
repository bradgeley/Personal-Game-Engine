// Bradley Christensen - 2022-2023
#include "GeometryUtils.h"
#include "MathUtils.h"
#include "Constants.h"
#include "AABB2.h"
#include "Plane2.h"
#include "Engine/Core/ErrorUtils.h"



//----------------------------------------------------------------------------------------------------------------------
bool DoDiscsOverlap2D(Vec2 const& position1, float radius1, Vec2 const& position2, float radius2)
{
    float distanceSquared = GetDistanceSquared2D(position1, position2);
    float combinedRadii = (radius1 + radius2);
    float combinedRadiiSquared = combinedRadii * combinedRadii;
    if (distanceSquared >= combinedRadiiSquared)
    {
        return false;
    }

    return true;
}



//----------------------------------------------------------------------------------------------------------------------
bool PushDiscOutOfDisc2D(Vec2& mobileDiscPos, float mobileDiscRadius, Vec2 const& staticDiscPos, float staticDiscRadius)
{
    Vec2 toMobileDisc = mobileDiscPos - staticDiscPos;
    float distanceSquared = toMobileDisc.GetLengthSquared();
    float combinedRadii = (mobileDiscRadius + staticDiscRadius);
    float combinedRadiiSquared = combinedRadii * combinedRadii;
    if (distanceSquared < combinedRadiiSquared)
    {
        float distance = SqrtF(distanceSquared);
        toMobileDisc /= distance;      // Normalize
        toMobileDisc *= combinedRadii; // Set length to combined radii
        mobileDiscPos = staticDiscPos + toMobileDisc;
        return true;
    }
    return false;
}



//----------------------------------------------------------------------------------------------------------------------
bool PushDiscsOutOfEachOther2D(Vec2& discPosA, float discRadiusA, Vec2& discPosB, float discRadiusB)
{
    Vec2 AtoB = discPosB - discPosA;
    float distanceSquared = AtoB.GetLengthSquared();
    float combinedRadii = (discRadiusA + discRadiusB);
    float combinedRadiiSquared = combinedRadii * combinedRadii;
    if (distanceSquared < combinedRadiiSquared)
    {
        float distance = SqrtF(distanceSquared);
        float overlapAmount = combinedRadii - distance;

        AtoB /= distance; // Normalize

        float weightA = discRadiusB / combinedRadii;
        float weightB = 1.f - weightA;

        Vec2 displacementA = AtoB * -1.f * overlapAmount * weightA;
        Vec2 displacementB = AtoB * overlapAmount * weightB;

        discPosA += displacementA;
        discPosB += displacementB;

        return true;
    }
    return false;
}



//----------------------------------------------------------------------------------------------------------------------
bool BounceDiscsOffEachOther2D(Vec2& discPosA, float discRadiusA, Vec2& discVelA, Vec2& discPosB, float discRadiusB, Vec2& discVelB, float elasticity)
{
    Vec2 AtoB = discPosB - discPosA;
    float distanceSquared = AtoB.GetLengthSquared();
    float combinedRadii = (discRadiusA + discRadiusB);
    float combinedRadiiSquared = combinedRadii * combinedRadii;
    if (distanceSquared < combinedRadiiSquared)
    {
        // Push discs out of each other first (no prediction yet)
        float distance = SqrtF(distanceSquared);
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
        float dotB = DotProduct2D(relativeVelB, AtoB);
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

        // Add back what we originally took away

        // Todo: prediction
        
        // Determine the real time of collision by assuming where the disc was last frame, using the quadratic equation

        // a = (Vx^2 + Vy^2)
        

        return true;
    }
    return false;
}



//----------------------------------------------------------------------------------------------------------------------
bool BounceDiscsOffEachOther2D(Vec2& discPosA, float discRadiusA, Vec2& discVelA, float massA, Vec2& discPosB, float discRadiusB, Vec2& discVelB, float massB, float elasticity)
{
    Vec2 AtoB = discPosB - discPosA;
    float distanceSquared = AtoB.GetLengthSquared();
    float combinedRadii = (discRadiusA + discRadiusB);
    float combinedRadiiSquared = combinedRadii * combinedRadii;
    if (distanceSquared < combinedRadiiSquared)
    {
        // Push discs out of each other first (no prediction yet)
        float distance = SqrtF(distanceSquared);
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
        
        float A_speedNorm = DotProduct2D(discVelA, collisionNormal);
        float B_speedNorm = DotProduct2D(discVelB, collisionNormal);
        float speedDiffInitial = A_speedNorm - B_speedNorm;
        if (speedDiffInitial > 0.f)
        {
            Vec2 velA_i = A_speedNorm * collisionNormal;
            Vec2 velB_i = B_speedNorm * collisionNormal;
            Vec2 velA_j = discVelA - velA_i;
            Vec2 velB_j = discVelB - velB_i;

            float halfSpeedDiffFinal = (0.5f) * elasticity * speedDiffInitial;
            float finalVelocity = (massA * A_speedNorm + massB * B_speedNorm) * oneOverCombinedMass;
            float A_speedNormF = finalVelocity - halfSpeedDiffFinal;
            float B_speedNormF = finalVelocity + halfSpeedDiffFinal;

            discVelA = A_speedNormF * collisionNormal + velA_j;
            discVelB = B_speedNormF * collisionNormal + velB_j;

            return true;
        }
    }
    return false;
}



//----------------------------------------------------------------------------------------------------------------------
bool DoLinesIntersect(Vec2 const& a1, Vec2 const& a2, Vec2 const& b1, Vec2 const& b2)
{
    Plane2 planeA(a1, a2);
    Plane2 planeB(b1, b2);
	return (planeA.Straddles(b1, b2)) && (planeB.Straddles(a1, a2));
}



//----------------------------------------------------------------------------------------------------------------------
bool DoesLineIntersectAABB2(Vec2 const& lineStart, Vec2 const& lineEnd, AABB2 const& box2D)
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

    if (DoLinesIntersect(topLeft, topRight, lineStart, lineEnd)) return true;
    if (DoLinesIntersect(topRight, botRight, lineStart, lineEnd)) return true;
    if (DoLinesIntersect(botRight, botLeft, lineStart, lineEnd)) return true;
    if (DoLinesIntersect(botLeft, topLeft, lineStart, lineEnd)) return true;
    
    return false;
}
