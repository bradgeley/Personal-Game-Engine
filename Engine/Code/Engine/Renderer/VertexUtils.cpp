// Bradley Christensen - 2022-2025
#include "Engine/Renderer/VertexUtils.h"
#include "Engine/Renderer/VertexBuffer.h"
#include "Engine/Math/AABB2.h"
#include "Engine/Math/IntVec2.h"
#include "Engine\Core\ErrorUtils.h"
#include "Engine\Core\StringUtils.h"
#include "Engine/Math/MathUtils.h"



//----------------------------------------------------------------------------------------------------------------------
void VertexUtils::AddVertsForLine2D(VertexBuffer& out_verts, Vec2 const& start, Vec2 const& end, float thickness, Rgba8 const& tint)
{
    float halfThickness = thickness * 0.5f;
    Vec2 lineDir = (end - start).GetNormalized();
    Vec2 lineRotated90 = lineDir.GetRotated90();

    // Could be rotated but using an upright line as example for naming
    Vec2 botLeftCorner  = start - lineDir * halfThickness + lineRotated90 * halfThickness;
    Vec2 botRightCorner = start - lineDir * halfThickness - lineRotated90 * halfThickness;
    Vec2 topRightCorner = end + lineDir * halfThickness - lineRotated90 * halfThickness;
    Vec2 topLeftCorner  = end + lineDir * halfThickness + lineRotated90 * halfThickness;
    
    // Push some verts
	out_verts.AddVert(Vertex_PCU(Vec3(botLeftCorner), tint));
	out_verts.AddVert(Vertex_PCU(Vec3(botRightCorner), tint));
	out_verts.AddVert(Vertex_PCU(Vec3(topRightCorner), tint));

	out_verts.AddVert(Vertex_PCU(Vec3(topRightCorner), tint));
	out_verts.AddVert(Vertex_PCU(Vec3(topLeftCorner), tint));
	out_verts.AddVert(Vertex_PCU(Vec3(botLeftCorner), tint));
}



//----------------------------------------------------------------------------------------------------------------------
void VertexUtils::AddVertsForArrow2D(VertexBuffer& out_verts, Vec2 const& start, Vec2 const& end, float thickness, Rgba8 const& tint)
{
    float halfThickness = thickness * 0.5f;
    Vec2 lineDir = (end - start).GetNormalized();
    Vec2 lineRotated90 = lineDir.GetRotated90();

    float tipThickness = thickness * 0.866f; // ~root(3) / 2
    float oneOverRootThree = 0.57735026f; // ~ 1 / root(3)

    // Could be rotated but using an upright line as example for naming
    Vec2 topMiddlePoint = end - lineDir * tipThickness;
    Vec2 botLeftCorner = start + lineRotated90 * halfThickness;
    Vec2 botRightCorner = start - lineRotated90 * halfThickness;
    Vec2 topRightCorner = topMiddlePoint - lineRotated90 * halfThickness;
    Vec2 topLeftCorner = topMiddlePoint + lineRotated90 * halfThickness;

    float tipLength = 2.5f * thickness;
    float baseThickness = tipLength * oneOverRootThree;
    Vec2 leftWingTip = end - lineDir * tipLength + lineRotated90 * baseThickness;
    Vec2 rightWingTip = end - lineDir * tipLength - lineRotated90 * baseThickness;

    // Push some verts
	out_verts.AddVert(Vertex_PCU(Vec3(end), tint));
	out_verts.AddVert(Vertex_PCU(Vec3(leftWingTip), tint));
	out_verts.AddVert(Vertex_PCU(Vec3(rightWingTip), tint));

	out_verts.AddVert(Vertex_PCU(Vec3(botLeftCorner), tint));
	out_verts.AddVert(Vertex_PCU(Vec3(botRightCorner), tint));
	out_verts.AddVert(Vertex_PCU(Vec3(topRightCorner), tint));

	out_verts.AddVert(Vertex_PCU(Vec3(topRightCorner), tint));
	out_verts.AddVert(Vertex_PCU(Vec3(topLeftCorner), tint));
	out_verts.AddVert(Vertex_PCU(Vec3(botLeftCorner), tint));
}



//----------------------------------------------------------------------------------------------------------------------
void VertexUtils::AddVertsForAABB2(VertexBuffer& out_verts, AABB2 const& square, Rgba8 const& tint, AABB2 const& UVs)
{
    AddVertsForRect2D(out_verts, square.mins, square.maxs, tint, UVs);
}



//----------------------------------------------------------------------------------------------------------------------
void VertexUtils::AddVertsForRect2D(VertexBuffer& out_verts, Vec2 const& mins, Vec2 const& maxs, Rgba8 const& tint, AABB2 const& UVs /*= AABB2::ZeroToOne*/, float z /*= 0.f*/)
{
    // Get corners
    Vec3 bottomRightPoint = Vec3(maxs.x, mins.y, z);
    Vec3 topRightPoint = Vec3(maxs.x, maxs.y, z);
    Vec3 bottomLeftPoint = Vec3(mins.x, mins.y, z);
    Vec3 topLeftPoint = Vec3(mins.x, maxs.y, z);

    // Get UVs
    Vec2 const& topRightUVs = UVs.maxs;
    Vec2 const& bottomLeftUVs = UVs.mins;
    Vec2 bottomRightUVs = Vec2(UVs.maxs.x, UVs.mins.y);
    Vec2 topLeftUVs = Vec2(UVs.mins.x, UVs.maxs.y);

    // Push some verts
    out_verts.AddVert(Vertex_PCU(bottomLeftPoint, tint, bottomLeftUVs));
    out_verts.AddVert(Vertex_PCU(bottomRightPoint, tint, bottomRightUVs));
    out_verts.AddVert(Vertex_PCU(topRightPoint, tint, topRightUVs));

    out_verts.AddVert(Vertex_PCU(topRightPoint, tint, topRightUVs));
    out_verts.AddVert(Vertex_PCU(topLeftPoint, tint, topLeftUVs));
    out_verts.AddVert(Vertex_PCU(bottomLeftPoint, tint, bottomLeftUVs));
}



//----------------------------------------------------------------------------------------------------------------------
void VertexUtils::AddVertsForWireBox2D(VertexBuffer& out_verts, AABB2 const& box, float lineThickness, Rgba8 const& tint)
{
    AddVertsForWireBox2D(out_verts, box.mins, box.maxs, lineThickness, tint);
}



//----------------------------------------------------------------------------------------------------------------------
void VertexUtils::AddVertsForWireBox2D(VertexBuffer& out_verts, Vec2 const& mins, Vec2 const& maxs, float lineThickness, Rgba8 const& tint)
{
    // Get corners
    Vec2 const& topRightPoint = maxs;
    Vec2 const& bottomLeftPoint = mins;
    Vec2 bottomRightPoint = Vec2(maxs.x, mins.y);
    Vec2 topLeftPoint = Vec2(mins.x, maxs.y);

    // Top and bottom
    AddVertsForLine2D(out_verts, topLeftPoint, topRightPoint, lineThickness, tint);
    AddVertsForLine2D(out_verts, bottomLeftPoint, bottomRightPoint, lineThickness, tint);

    // Left and right (dropped down by half thickness)
    Vec2 offset = Vec2(0.f, lineThickness);
    AddVertsForLine2D(out_verts, topLeftPoint - offset, bottomLeftPoint + offset, lineThickness, tint);
    AddVertsForLine2D(out_verts, topRightPoint - offset, bottomRightPoint + offset, lineThickness, tint);
}



//----------------------------------------------------------------------------------------------------------------------
// Add lines for columns and rows to create a grid
// There will be some overlap where lines intersect, but  
//
void VertexUtils::AddVertsForWireGrid(VertexBuffer& out_verts, AABB2 const& boundingAABB, IntVec2 const& dims, float lineThickness, Rgba8 const& tint)
{
    ASSERT_OR_DIE(dims.x != 0 && dims.y != 0, "Cannot add verts for a grid with 0 for one of its dimensions.")
    Vec2 cellDims = boundingAABB.GetDimensions() / Vec2(dims);

    // Add lines for columns
    for (int x = 0; x <= dims.x; ++x)
    {
        float cellPosX = x * cellDims.x;
        Vec2 bot = boundingAABB.mins + Vec2(cellPosX, 0.f);
        Vec2 top = boundingAABB.GetTopLeft() + Vec2(cellPosX, 0.f);
        AddVertsForLine2D(out_verts, top, bot, lineThickness, tint);
    }

    // Add lines for rows
	for (int y = 0; y <= dims.y; ++y)
	{
		float cellPosY = y * cellDims.y;
		Vec2 left = boundingAABB.mins + Vec2(0.f, cellPosY);
		Vec2 right = boundingAABB.GetBottomRight() + Vec2(0.f, cellPosY);
		AddVertsForLine2D(out_verts, left, right, lineThickness, tint);
	}
}



//----------------------------------------------------------------------------------------------------------------------
void VertexUtils::AddVertsForGrid(VertexBuffer& out_verts, AABB2 const& boundingAABB, IntVec2 const& dims, Rgba8 const& tint)
{
    ASSERT_OR_DIE(dims.x != 0 && dims.y != 0, "Cannot add verts for a grid with 0 for one of its dimensions.")

    Vec2 cellDims = boundingAABB.GetDimensions() / Vec2(dims);

    for (int y = 0; y < dims.y; ++y)
    {
        for (int x = 0; x < dims.x; ++x)
        {
            Vec2 coords = Vec2(x, y);
            Vec2 boxMins = boundingAABB.mins + cellDims * coords;
            AABB2 box = AABB2(boxMins, boxMins + cellDims);
            AddVertsForAABB2(out_verts, box, tint);
        }
    }
}



//----------------------------------------------------------------------------------------------------------------------
void VertexUtils::AddVertsForGrid(VertexBuffer& out_verts, AABB2 const& boundingAABB, Rgba8 const* colorsRowMajor, IntVec2 const& dims, Rgba8 const& gridLinesTint)
{
    ASSERT_OR_DIE(dims.x != 0 && dims.y != 0, "Cannot add verts for a grid with 0 for one of its dimensions.")

    Vec2 cellDims = boundingAABB.GetDimensions() / Vec2(dims);
    float lineThickness = 0.1f * MathUtils::Min(cellDims.x, cellDims.y);

    for (int y = 0; y < dims.y; ++y)
    {
        for (int x = 0; x < dims.x; ++x)
        {
            Vec2 coords = Vec2(x, y);
            Vec2 boxMins = boundingAABB.mins + cellDims * coords;
            AABB2 box = AABB2(boxMins, boxMins + cellDims);
            AddVertsForAABB2(out_verts, box, colorsRowMajor[y * dims.x + x]);
        }
    }

    if (gridLinesTint.a > 0)
    {
        AddVertsForWireGrid(out_verts, boundingAABB, dims, lineThickness, gridLinesTint);
    }
}



//----------------------------------------------------------------------------------------------------------------------
void VertexUtils::AddVertsForDisc2D(VertexBuffer& out_verts, Vec2 const& center, float radius, int numSides, Rgba8 const& tint, AABB2 const& UVs)
{
    ASSERT_OR_DIE(radius > 0.f && numSides >= 3, StringUtils::StringF("Cannot add verts for a disc with: radius=%f numSides=%i", radius, numSides));

    Vec2 uvCenter = UVs.GetCenter();

    float thetaStepSize = 360.f / (float) numSides;
    for (int i = 0; i < numSides; ++i)
    {
        float theta1 = i * thetaStepSize;
        float theta2 = (i + 1) * thetaStepSize;

        Vec2 unitCirclePos1 = Vec2::MakeFromUnitCircleDegrees(theta1);
        Vec2 unitCirclePos2 = Vec2::MakeFromUnitCircleDegrees(theta2);

        Vec2 cornerPt1 = center + unitCirclePos1 * radius;
        Vec2 cornerPt2 = center + unitCirclePos2 * radius;

        Vec2 uv1 = uvCenter + unitCirclePos1 * 0.5f;
        Vec2 uv2 = uvCenter + unitCirclePos2 * 0.5f;

        out_verts.AddVert(Vertex_PCU(center, tint, uvCenter));
		out_verts.AddVert(Vertex_PCU(cornerPt1, tint, uv1));
		out_verts.AddVert(Vertex_PCU(cornerPt2, tint, uv2));
    }
}



//----------------------------------------------------------------------------------------------------------------------
void VertexUtils::AddVertsForWireDisc2D(VertexBuffer& out_verts, Vec2 const& center, float radius, float thickness, int numSides, Rgba8 const& tint)
{
    ASSERT_OR_DIE(radius > 0.f && numSides >= 3, StringUtils::StringF("Cannot add verts for a disc with: radius=%f numSides=%i", radius, numSides));
    float halfThickness = thickness * 0.5f;
    float innerRadius = radius - halfThickness;
    float outerRadius = radius + halfThickness;
    float thetaStepSize = 360.f / (float)numSides;

	std::vector<Vertex_PCU> verts;
	verts.reserve(numSides * 6);
    for (int i = 0; i < numSides; ++i)
    {
        float theta1 = i * thetaStepSize;
        float theta2 = (i + 1) * thetaStepSize;
        Vec2 unitCirclePos1 = Vec2::MakeFromUnitCircleDegrees(theta1);
        Vec2 unitCirclePos2 = Vec2::MakeFromUnitCircleDegrees(theta2);
        Vec2 innerPt1 = center + unitCirclePos1 * innerRadius;
        Vec2 innerPt2 = center + unitCirclePos2 * innerRadius;
        Vec2 outerPt1 = center + unitCirclePos1 * outerRadius;
        Vec2 outerPt2 = center + unitCirclePos2 * outerRadius;

		verts.emplace_back(Vec3(innerPt1), tint);
		verts.emplace_back(Vec3(outerPt1), tint);
		verts.emplace_back(Vec3(outerPt2), tint);
		verts.emplace_back(Vec3(outerPt2), tint);
		verts.emplace_back(Vec3(innerPt2), tint);
        verts.emplace_back(Vec3(innerPt1), tint);
	}

	out_verts.AddVerts(verts);
}



//----------------------------------------------------------------------------------------------------------------------
void VertexUtils::AddVertsForCapsule2D(VertexBuffer& out_verts, Vec2 const& start, Vec2 const& end, float radius, Rgba8 const& tint)
{
    if (radius <= 0)
    {
        return;
    }


    constexpr int numSidesOnEachEnd = 64;

    Vec2 lineDir = (end - start).GetNormalized();
    Vec2 lineRotated90 = lineDir.GetRotated90();

    // Could be rotated but using an upright capsule as example for naming
    Vec2 botLeftCorner = start + lineRotated90 * radius;
    Vec2 botRightCorner = start - lineRotated90 * radius;
    Vec2 topRightCorner = end - lineRotated90 * radius;
    Vec2 topLeftCorner = end + lineRotated90 * radius;

	out_verts.AddVert(Vertex_PCU(Vec3(botLeftCorner), tint));
	out_verts.AddVert(Vertex_PCU(Vec3(botRightCorner), tint));
	out_verts.AddVert(Vertex_PCU(Vec3(topRightCorner), tint));
    
	out_verts.AddVert(Vertex_PCU(Vec3(topRightCorner), tint));
	out_verts.AddVert(Vertex_PCU(Vec3(topLeftCorner), tint));
	out_verts.AddVert(Vertex_PCU(Vec3(botLeftCorner), tint));

    float startingTheta = lineRotated90.GetAngleDegrees();
    float thetaStepSize = 180.f / (float) numSidesOnEachEnd;
    for (int i = 0; i < numSidesOnEachEnd; ++i)
    {
        float theta1 = startingTheta + i * thetaStepSize;
        float theta2 = startingTheta + (i + 1) * thetaStepSize;

        Vec2 unitCirclePos1 = Vec2::MakeFromUnitCircleDegrees(theta1);
        Vec2 unitCirclePos2 = Vec2::MakeFromUnitCircleDegrees(theta2);

        Vec2 cornerPt1 = start + unitCirclePos1 * radius;
        Vec2 cornerPt2 = start + unitCirclePos2 * radius;

		out_verts.AddVert(Vertex_PCU(Vec3(start), tint));
		out_verts.AddVert(Vertex_PCU(Vec3(cornerPt1), tint));
		out_verts.AddVert(Vertex_PCU(Vec3(cornerPt2), tint));
    }

    startingTheta += 180.f;
    for (int i = 0; i < numSidesOnEachEnd; ++i)
    {
        float theta1 = startingTheta + i * thetaStepSize;
        float theta2 = startingTheta + (i + 1) * thetaStepSize;

        Vec2 unitCirclePos1 = Vec2::MakeFromUnitCircleDegrees(theta1);
        Vec2 unitCirclePos2 = Vec2::MakeFromUnitCircleDegrees(theta2);

        Vec2 cornerPt1 = end + unitCirclePos1 * radius;
        Vec2 cornerPt2 = end + unitCirclePos2 * radius;

		out_verts.AddVert(Vertex_PCU(Vec3(end), tint));
		out_verts.AddVert(Vertex_PCU(Vec3(cornerPt1), tint));
		out_verts.AddVert(Vertex_PCU(Vec3(cornerPt2), tint));
    }
}



//----------------------------------------------------------------------------------------------------------------------
void VertexUtils::AddVertsForWireMesh2D(VertexBuffer& out_verts, VertexBuffer const& triangles, float thickness, Rgba8 const& tint)
{
    for (size_t i = 0; i < triangles.GetNumVerts(); i += 3)
    {
        Vertex_PCU const& v1 = triangles.GetVert<Vertex_PCU>(i);
        Vertex_PCU const& v2 = triangles.GetVert<Vertex_PCU>(i + 1);
        Vertex_PCU const& v3 = triangles.GetVert<Vertex_PCU>(i + 2);

        AddVertsForLine2D(out_verts, Vec2(v1.pos), Vec2(v2.pos), thickness, tint);
        AddVertsForLine2D(out_verts, Vec2(v2.pos), Vec2(v3.pos), thickness, tint);
        AddVertsForLine2D(out_verts, Vec2(v3.pos), Vec2(v1.pos), thickness, tint);
    }
}



//----------------------------------------------------------------------------------------------------------------------
int VertexUtils::WriteVertsForRect2D(Vertex_PCU* writeLocation, Vec2 const& mins, Vec2 const& maxs, Rgba8 const& tint, AABB2 const& UVs, float z)
{
    // Get corners
    Vec3 bottomRightPoint = Vec3(maxs.x, mins.y, z);
    Vec3 topRightPoint = Vec3(maxs.x, maxs.y, z);
    Vec3 bottomLeftPoint = Vec3(mins.x, mins.y, z);
    Vec3 topLeftPoint = Vec3(mins.x, maxs.y, z);

    // Get UVs
    Vec2 const& topRightUVs = UVs.maxs;
    Vec2 const& bottomLeftUVs = UVs.mins;
    Vec2 bottomRightUVs = Vec2(UVs.maxs.x, UVs.mins.y);
    Vec2 topLeftUVs = Vec2(UVs.mins.x, UVs.maxs.y);

    // Write verts
    writeLocation[0] = (Vertex_PCU(bottomLeftPoint, tint, bottomLeftUVs));
    writeLocation[1] = (Vertex_PCU(bottomRightPoint, tint, bottomRightUVs));
    writeLocation[2] = (Vertex_PCU(topRightPoint, tint, topRightUVs));

    writeLocation[3] = (Vertex_PCU(topRightPoint, tint, topRightUVs));
    writeLocation[4] = (Vertex_PCU(topLeftPoint, tint, topLeftUVs));
    writeLocation[5] = (Vertex_PCU(bottomLeftPoint, tint, bottomLeftUVs));
    return 6;
}
