// Bradley Christensen - 2022-2023
#include "GeometryUtils.h"
#include "AABB2.h"
#include "Plane2.h"



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
