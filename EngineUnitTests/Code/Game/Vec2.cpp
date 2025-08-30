// Bradley Christensen 2022-2025
#include "pch.h"
#include "Math/Vec2.h"

//----------------------------------------------------------------------------------------------------------------------
// Vec2 Unit Tests
//

// Addition
TEST(Vec2, Addition)
{
    Vec2 a(3.0f, 4.0f);
    Vec2 b(1.0f, 2.0f);
    Vec2 c = a + b;
    EXPECT_EQ(c, Vec2(4.0f, 6.0f));

    Vec2 d(-5.0f, 10.0f);
    Vec2 e(2.0f, -3.0f);
    EXPECT_EQ(d + e, Vec2(-3.0f, 7.0f));

    Vec2 f(1e6f, -1e-6f);
    Vec2 g(-1e6f, 1e-6f);
    EXPECT_EQ(f + g, Vec2(0.0f, 0.0f));
}

// Subtraction
TEST(Vec2, Subtraction)
{
    Vec2 a(3.0f, 4.0f);
    Vec2 b(1.0f, 2.0f);
    Vec2 c = a - b;
    EXPECT_EQ(c, Vec2(2.0f, 2.0f));

    Vec2 d(-5.0f, 10.0f);
    Vec2 e(2.0f, -3.0f);
    EXPECT_EQ(d - e, Vec2(-7.0f, 13.0f));

    Vec2 f(1e6f, -1e-6f);
    Vec2 g(-1e6f, 1e-6f);
    EXPECT_EQ(f - g, Vec2(2e6f, -2e-6f));
}

// Scalar Multiplication
TEST(Vec2, ScalarMultiplication)
{
    Vec2 a(3.0f, 4.0f);
    Vec2 b = a * 2.0f;
    EXPECT_EQ(b, Vec2(6.0f, 8.0f));

    Vec2 c(-5.0f, 10.0f);
    EXPECT_EQ(c * -2.0f, Vec2(10.0f, -20.0f));

    Vec2 d(1e-3f, 1e3f);
    EXPECT_EQ(d * 1e3f, Vec2(1.0f, 1e6f));

    // Test float * Vec2
    Vec2 e = 3.0f * a;
    EXPECT_EQ(e, Vec2(9.0f, 12.0f));
}

// Scalar Division
TEST(Vec2, ScalarDivision)
{
    Vec2 a(3.0f, 4.0f);
    Vec2 b = a / 2.0f;
    EXPECT_EQ(b, Vec2(1.5f, 2.0f));

    Vec2 c(-5.0f, 10.0f);
    EXPECT_EQ(c / -5.0f, Vec2(1.0f, -2.0f));

    Vec2 d(1e6f, 1e-6f);
    EXPECT_EQ(d / 1e2f, Vec2(1e4f, 1e-8f));
}

// Dot Product
TEST(Vec2, DotProduct)
{
    Vec2 a(3.0f, 4.0f);
    Vec2 b(1.0f, 2.0f);
    float dot = a.Dot(b);
    EXPECT_EQ(dot, 11.0f);

    Vec2 c(-5.0f, 10.0f);
    Vec2 d(2.0f, -3.0f);
    EXPECT_EQ(c.Dot(d), -40.0f);

    Vec2 e(1e3f, 1e-3f);
    Vec2 f(1e-3f, 1e3f);
    EXPECT_FLOAT_EQ(e.Dot(f), 2.0f);
}

// Cross Product
TEST(Vec2, CrossProduct)
{
    Vec2 a(3.0f, 4.0f);
    Vec2 b(1.0f, 2.0f);
    EXPECT_EQ(a.Cross(b), 2.0f);

    Vec2 c(-5.0f, 10.0f);
    Vec2 d(2.0f, -3.0f);
    EXPECT_EQ(c.Cross(d), -5.0f);

    Vec2 e(1e3f, 1e-3f);
    Vec2 f(1e-3f, 1e3f);
    EXPECT_FLOAT_EQ(e.Cross(f), 1e6f - 1e-6f);
}

// Length
TEST(Vec2, GetLength)
{
    Vec2 a(3.0f, 4.0f);
    EXPECT_FLOAT_EQ(a.GetLength(), 5.0f);

    Vec2 b(-5.0f, 12.0f);
    EXPECT_FLOAT_EQ(b.GetLength(), 13.0f);

    Vec2 c(1e-3f, 0.0f);
    EXPECT_FLOAT_EQ(c.GetLength(), 1e-3f);
}

// LengthSquared
TEST(Vec2, GetLengthSquared)
{
    Vec2 a(3.0f, 4.0f);
    EXPECT_FLOAT_EQ(a.GetLengthSquared(), 25.0f);

    Vec2 b(-5.0f, 12.0f);
    EXPECT_FLOAT_EQ(b.GetLengthSquared(), 169.0f);

    Vec2 c(1e-3f, 0.0f);
    EXPECT_FLOAT_EQ(c.GetLengthSquared(), 1e-6f);
}

// AngleDegrees
TEST(Vec2, GetAngleDegrees)
{
    Vec2 a(1.0f, 0.0f);
    EXPECT_FLOAT_EQ(a.GetAngleDegrees(), 0.0f);

    Vec2 b(0.0f, 1.0f);
    EXPECT_FLOAT_EQ(b.GetAngleDegrees(), 90.0f);

    Vec2 c(-1.0f, 0.0f);
    EXPECT_FLOAT_EQ(c.GetAngleDegrees(), 180.0f);

    Vec2 d(0.0f, -1.0f);
    EXPECT_FLOAT_EQ(d.GetAngleDegrees(), -90.0f);
}

// Normalize
TEST(Vec2, Normalize)
{
    Vec2 a(3.0f, 4.0f);
    a.Normalize();
    EXPECT_FLOAT_EQ(a.x, 0.6f);
    EXPECT_FLOAT_EQ(a.y, 0.8f);

    Vec2 b(-5.0f, 0.0f);
    b.Normalize();
    EXPECT_FLOAT_EQ(b.x, -1.0f);
    EXPECT_FLOAT_EQ(b.y, 0.0f);
}

// GetNormalized
TEST(Vec2, GetNormalized)
{
    Vec2 a(3.0f, 4.0f);
    Vec2 g = a.GetNormalized();
    EXPECT_FLOAT_EQ(g.x, 0.6f);
    EXPECT_FLOAT_EQ(g.y, 0.8f);

    Vec2 b(0.0f, -2.0f);
    Vec2 h = b.GetNormalized();
    EXPECT_FLOAT_EQ(h.x, 0.0f);
    EXPECT_FLOAT_EQ(h.y, -1.0f);
}

// GetRotated90
TEST(Vec2, GetRotated90)
{
    Vec2 a(1.0f, 2.0f);
    Vec2 b = a.GetRotated90();
    EXPECT_EQ(b, Vec2(-2.0f, 1.0f));

    Vec2 c(-3.0f, 4.0f);
    EXPECT_EQ(c.GetRotated90(), Vec2(-4.0f, -3.0f));
}

// GetRotatedMinus90
TEST(Vec2, GetRotatedMinus90)
{
    Vec2 a(1.0f, 2.0f);
    Vec2 b = a.GetRotatedMinus90();
    EXPECT_EQ(b, Vec2(2.0f, -1.0f));

    Vec2 c(-3.0f, 4.0f);
    EXPECT_EQ(c.GetRotatedMinus90(), Vec2(4.0f, 3.0f));
}

// GetRotated
TEST(Vec2, GetRotated)
{
    Vec2 a(1.0f, 0.0f);
    Vec2 b = a.GetRotated(90.0f);
    EXPECT_NEAR(b.x, 0.0f, 1e-6f);
    EXPECT_NEAR(b.y, 1.0f, 1e-6f);

    Vec2 c(0.0f, 1.0f);
    Vec2 d = c.GetRotated(-90.0f);
    EXPECT_NEAR(d.x, 1.0f, 1e-6f);
    EXPECT_NEAR(d.y, 0.0f, 1e-6f);
}

// GetProjectedOntoNormal
TEST(Vec2, GetProjectedOntoNormal)
{
    Vec2 a(3.0f, 4.0f);
    Vec2 normal(0.0f, 1.0f);
    Vec2 proj = a.GetProjectedOntoNormal(normal);
    EXPECT_EQ(proj, Vec2(0.0f, 4.0f));

    Vec2 b(5.0f, 0.0f);
    Vec2 n2(1.0f, 0.0f);
    EXPECT_EQ(b.GetProjectedOntoNormal(n2), Vec2(5.0f, 0.0f));
}

// GetProjectedOnto
TEST(Vec2, GetProjectedOnto)
{
    Vec2 a(3.0f, 4.0f);
    Vec2 b(1.0f, 0.0f);
    Vec2 proj = a.GetProjectedOnto(b);
    EXPECT_NEAR(proj.x, 3.0f, 1e-6f);
    EXPECT_NEAR(proj.y, 0.0f, 1e-6f);

    Vec2 c(0.0f, 5.0f);
    Vec2 d(0.0f, 2.0f);
    Vec2 proj2 = c.GetProjectedOnto(d);
    EXPECT_NEAR(proj2.x, 0.0f, 1e-6f);
    EXPECT_NEAR(proj2.y, 5.0f, 1e-6f);
}

// GetFloor
TEST(Vec2, GetFloor)
{
    Vec2 a(3.7f, 4.2f);
    Vec2 b = a.GetFloor();
    EXPECT_EQ(b, Vec2(3.0f, 4.0f));

    Vec2 c(-1.2f, -3.8f);
    EXPECT_EQ(c.GetFloor(), Vec2(-2.0f, -4.0f));
}

// GetDistanceTo
TEST(Vec2, GetDistanceTo)
{
    Vec2 a(0.0f, 0.0f);
    Vec2 b(3.0f, 4.0f);
    EXPECT_FLOAT_EQ(a.GetDistanceTo(b), 5.0f);

    Vec2 c(-1.0f, -1.0f);
    Vec2 d(2.0f, 3.0f);
    EXPECT_FLOAT_EQ(c.GetDistanceTo(d), 5.0f);
}

// GetDistanceSquaredTo
TEST(Vec2, GetDistanceSquaredTo)
{
    Vec2 a(0.0f, 0.0f);
    Vec2 b(3.0f, 4.0f);
    EXPECT_FLOAT_EQ(a.GetDistanceSquaredTo(b), 25.0f);

    Vec2 c(-1.0f, -1.0f);
    Vec2 d(2.0f, 3.0f);
    EXPECT_FLOAT_EQ(c.GetDistanceSquaredTo(d), 25.0f);
}

// IsZero
TEST(Vec2, IsZero)
{
    Vec2 a(0.0f, 0.0f);
    EXPECT_TRUE(a.IsZero());

    Vec2 b(1e-6f, 0.0f);
    EXPECT_FALSE(b.IsZero());
}

// IsNearlyZero
TEST(Vec2, IsNearlyZero)
{
    Vec2 a(1e-7f, -1e-7f);
    EXPECT_TRUE(a.IsNearlyZero(1e-6f));

    Vec2 b(1e-3f, 0.0f);
    EXPECT_FALSE(b.IsNearlyZero(1e-6f));
}

// Rotate90
TEST(Vec2, Rotate90)
{
    Vec2 a(1.0f, 2.0f);
    a.Rotate90();
    EXPECT_EQ(a, Vec2(-2.0f, 1.0f));

    Vec2 b(-3.0f, 4.0f);
    b.Rotate90();
    EXPECT_EQ(b, Vec2(-4.0f, -3.0f));
}

// RotateMinus90
TEST(Vec2, RotateMinus90)
{
    Vec2 a(1.0f, 2.0f);
    a.RotateMinus90();
    EXPECT_EQ(a, Vec2(2.0f, -1.0f));

    Vec2 b(-3.0f, 4.0f);
    b.RotateMinus90();
    EXPECT_EQ(b, Vec2(4.0f, 3.0f));
}

// Rotate
TEST(Vec2, Rotate)
{
    Vec2 a(1.0f, 0.0f);
    a.Rotate(90.0f);
    EXPECT_NEAR(a.x, 0.0f, 1e-6f);
    EXPECT_NEAR(a.y, 1.0f, 1e-6f);

    Vec2 b(0.0f, 1.0f);
    b.Rotate(-90.0f);
    EXPECT_NEAR(b.x, 1.0f, 1e-6f);
    EXPECT_NEAR(b.y, 0.0f, 1e-6f);
}

// SetLength
TEST(Vec2, SetLength)
{
    Vec2 a(3.0f, 4.0f);
    a.SetLength(10.0f);
    EXPECT_NEAR(a.GetLength(), 10.0f, 1e-6f);

    Vec2 b(-5.0f, 0.0f);
    b.SetLength(2.0f);
    EXPECT_NEAR(b.GetLength(), 2.0f, 1e-6f);
}

// ClampLength
TEST(Vec2, ClampLength)
{
    Vec2 a(3.0f, 4.0f);
    a.ClampLength(3.0f);
    EXPECT_NEAR(a.GetLength(), 3.0f, 1e-6f);

    Vec2 b(1.0f, 1.0f);
    b.ClampLength(5.0f);
    EXPECT_NEAR(b.GetLength(), sqrtf(2.0f), 1e-6f);
}

// ClampLengthMin
TEST(Vec2, ClampLengthMin)
{
    Vec2 a(1.0f, 1.0f);
    a.ClampLengthMin(5.0f);
    EXPECT_NEAR(a.GetLength(), 5.0f, 1e-6f);

    Vec2 b(10.0f, 0.0f);
    b.ClampLengthMin(5.0f);
    EXPECT_NEAR(b.GetLength(), 10.0f, 1e-6f);
}

// ClampLength (min, max)
TEST(Vec2, ClampLengthMinMax)
{
    Vec2 a(1.0f, 1.0f);
    a.ClampLength(5.0f, 10.0f);
    EXPECT_NEAR(a.GetLength(), 5.0f, 1e-6f);

    Vec2 b(20.0f, 0.0f);
    b.ClampLength(5.0f, 10.0f);
    EXPECT_NEAR(b.GetLength(), 10.0f, 1e-6f);

    Vec2 c(7.0f, 0.0f);
    c.ClampLength(5.0f, 10.0f);
    EXPECT_NEAR(c.GetLength(), 7.0f, 1e-6f);
}

// Floor
TEST(Vec2, Floor)
{
    Vec2 a(3.7f, 4.2f);
    a.Floor();
    EXPECT_EQ(a, Vec2(3.0f, 4.0f));

    Vec2 b(-1.2f, -3.8f);
    b.Floor();
    EXPECT_EQ(b, Vec2(-2.0f, -4.0f));
}

// MakeFromUnitCircleDegrees
TEST(Vec2, MakeFromUnitCircleDegrees)
{
    Vec2 a = Vec2::MakeFromUnitCircleDegrees(0.0f);
    EXPECT_NEAR(a.x, 1.0f, 1e-6f);
    EXPECT_NEAR(a.y, 0.0f, 1e-6f);

    Vec2 b = Vec2::MakeFromUnitCircleDegrees(90.0f);
    EXPECT_NEAR(b.x, 0.0f, 1e-6f);
    EXPECT_NEAR(b.y, 1.0f, 1e-6f);
}

// MakeFromPolarCoords
TEST(Vec2, MakeFromPolarCoords)
{
    Vec2 a = Vec2::MakeFromPolarCoords(0.0f, 5.0f);
    EXPECT_NEAR(a.x, 5.0f, 1e-6f);
    EXPECT_NEAR(a.y, 0.0f, 1e-6f);

    Vec2 b = Vec2::MakeFromPolarCoords(90.0f, 2.0f);
    EXPECT_NEAR(b.x, 0.0f, 1e-6f);
    EXPECT_NEAR(b.y, 2.0f, 1e-6f);
}

// Operator overloads
TEST(Vec2, NegationOperator)
{
    Vec2 a(3.0f, -4.0f);
    Vec2 b = -a;
    EXPECT_EQ(b, Vec2(-3.0f, 4.0f));
}

TEST(Vec2, ComponentWiseMultiplication)
{
    Vec2 a(2.0f, 3.0f);
    Vec2 b(4.0f, -1.0f);
    EXPECT_EQ(a * b, Vec2(8.0f, -3.0f));
}

TEST(Vec2, ComponentWiseDivision)
{
    Vec2 a(8.0f, -3.0f);
    Vec2 b(2.0f, 3.0f);
    EXPECT_EQ(a / b, Vec2(4.0f, -1.0f));
}

TEST(Vec2, CompoundAssignmentOperators)
{
    Vec2 a(1.0f, 2.0f);
    Vec2 b(3.0f, 4.0f);
    a += b;
    EXPECT_EQ(a, Vec2(4.0f, 6.0f));

    a -= b;
    EXPECT_EQ(a, Vec2(1.0f, 2.0f));

    a *= b;
    EXPECT_EQ(a, Vec2(3.0f, 8.0f));

    a /= b;
    EXPECT_EQ(a, Vec2(1.0f, 2.0f));

    a *= 2.0f;
    EXPECT_EQ(a, Vec2(2.0f, 4.0f));

    a /= 2.0f;
    EXPECT_EQ(a, Vec2(1.0f, 2.0f));
}

TEST(Vec2, EqualityOperators)
{
    Vec2 a(1.0f, 2.0f);
    Vec2 b(1.0f, 2.0f);
    Vec2 c(2.0f, 1.0f);
    EXPECT_TRUE(a == b);
    EXPECT_FALSE(a != b);
    EXPECT_FALSE(a == c);
    EXPECT_TRUE(a != c);
}