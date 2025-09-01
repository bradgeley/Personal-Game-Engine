// Bradley Christensen 2022-2025
#include "pch.h"
#include "Engine/Math/Mat44.h"
#include "Engine/Math/Vec2.h"
#include "Engine/Math/Vec3.h"
#include <gtest/gtest.h>



//----------------------------------------------------------------------------------------------------------------------
// Mat44 Unit Tests
//
namespace TestMat44
{

    //----------------------------------------------------------------------------------------------------------------------
    // Test 1: Default Constructor and Identity
    //
    TEST(Mat44Tests, DefaultConstructorAndIdentity)
    {
        Mat44 m;
        EXPECT_EQ(m, Mat44::Identity);
        for (int i = 0; i < 16; ++i)
        {
            EXPECT_FLOAT_EQ(m.m_values[i], Mat44::Identity.m_values[i]);
        }
    }


    //----------------------------------------------------------------------------------------------------------------------
    // Test 2: 2D Basis Constructor
    //
    TEST(Mat44Tests, Constructor2DBasis)
    {
        Vec2 i(1.f, 2.f);
        Vec2 j(3.f, 4.f);
        Vec2 t(5.f, 6.f);
        Mat44 m(i, j, t);

        EXPECT_FLOAT_EQ(m.m_values[Mat44::Ix], 1.f);
        EXPECT_FLOAT_EQ(m.m_values[Mat44::Iy], 2.f);
        EXPECT_FLOAT_EQ(m.m_values[Mat44::Jx], 3.f);
        EXPECT_FLOAT_EQ(m.m_values[Mat44::Jy], 4.f);
        EXPECT_FLOAT_EQ(m.m_values[Mat44::Tx], 5.f);
        EXPECT_FLOAT_EQ(m.m_values[Mat44::Ty], 6.f);
        EXPECT_FLOAT_EQ(m.m_values[Mat44::Kz], 1.f);
        EXPECT_FLOAT_EQ(m.m_values[Mat44::Tw], 1.f);
    }


    //----------------------------------------------------------------------------------------------------------------------
    // Test 3: GetAsFloatArray
    //
    TEST(Mat44Tests, GetAsFloatArray)
    {
        Mat44 m;
        float* arr = m.GetAsFloatArray();
        const float* arrConst = static_cast<const Mat44&>(m).GetAsFloatArray();
        for (int i = 0; i < 16; ++i)
        {
            EXPECT_FLOAT_EQ(arr[i], m.m_values[i]);
            EXPECT_FLOAT_EQ(arrConst[i], m.m_values[i]);
        }
    }


    //----------------------------------------------------------------------------------------------------------------------
    // Test 4: Operator[]
    //
    TEST(Mat44Tests, OperatorIndex)
    {
        Mat44 m;

        m[0] = 42.f;
        EXPECT_FLOAT_EQ(m.m_values[0], 42.f);
        m[15] = -7.f;
        EXPECT_FLOAT_EQ(m.m_values[15], -7.f);
    }


    //----------------------------------------------------------------------------------------------------------------------
    // Test 5: Reset
    //
    TEST(Mat44Tests, Reset)
    {
        Mat44 m;
        m[0] = 99.f;
        m.Reset();
        EXPECT_EQ(m, Mat44::Identity);
    }


    //----------------------------------------------------------------------------------------------------------------------
    // Test 6: Append and GetAppended
    //
    TEST(Mat44Tests, AppendAndGetAppended)
    {
        Mat44 m = Mat44::CreateTranslation3D(1.f, 2.f, 3.f);
        Mat44 rot = Mat44::CreateZRotationDegrees(90.f);
        Mat44 scale = Mat44::CreateUniformScale3D(2.f);

        Mat44 mCopy = m;
        m.Append(rot);
        m.Append(scale);

        Mat44 appended = mCopy.GetAppended(rot).GetAppended(scale);

        for (int i = 0; i < 16; ++i)
        {
            EXPECT_NEAR(m.m_values[i], appended.m_values[i], 1e-4f);
        }
    }


    //----------------------------------------------------------------------------------------------------------------------
    // Test 7: Append Multiple Mat44s
    //
    TEST(Mat44Tests, AppendMultiple)
    {
        Mat44 m = Mat44::Identity;
        Mat44 t = Mat44::CreateTranslation3D(5.f, 0.f, 0.f);
        Mat44 r = Mat44::CreateYRotationDegrees(90.f);
        Mat44 s = Mat44::CreateUniformScale3D(3.f);

        m.Append(t);
        m.Append(r);
        m.Append(s);

        Mat44 expected = Mat44::Identity;
        expected = expected.GetAppended(t).GetAppended(r).GetAppended(s);

        for (int i = 0; i < 16; ++i)
        {
            EXPECT_NEAR(m.m_values[i], expected.m_values[i], 1e-4f);
        }
    }


    //----------------------------------------------------------------------------------------------------------------------
    // Test 8: Transpose
    //
    TEST(Mat44Tests, Transpose)
    {
        Mat44 m;
        m[Mat44::Ix] = 1.f; m[Mat44::Jx] = 2.f; m[Mat44::Kx] = 3.f; m[Mat44::Tx] = 4.f;
        m[Mat44::Iy] = 5.f; m[Mat44::Jy] = 6.f; m[Mat44::Ky] = 7.f; m[Mat44::Ty] = 8.f;
        m[Mat44::Iz] = 9.f; m[Mat44::Jz] = 10.f; m[Mat44::Kz] = 11.f; m[Mat44::Tz] = 12.f;
        m[Mat44::Iw] = 13.f; m[Mat44::Jw] = 14.f; m[Mat44::Kw] = 15.f; m[Mat44::Tw] = 16.f;

        Mat44 mt = m;
        mt.Transpose();

        for (int row = 0; row < 4; ++row)
        {
            for (int col = 0; col < 4; ++col)
            {
                EXPECT_FLOAT_EQ(mt.m_values[row * 4 + col], m.m_values[col * 4 + row]);
            }
        }
    }


    //----------------------------------------------------------------------------------------------------------------------
    // Test 9: InvertOrthoNormal and GetOrthoNormalInverse
    //
    TEST(Mat44Tests, InvertOrthoNormal)
    {
        Mat44 m = Mat44::CreateTranslation3D(1.f, 2.f, 3.f);
        m.Append(Mat44::CreateZRotationDegrees(90.f));
        Mat44 inv = m.GetOrthoNormalInverse();

        Mat44 mCopy = m;
        mCopy.InvertOrthoNormal();

        for (int i = 0; i < 16; ++i)
        {
            EXPECT_NEAR(inv.m_values[i], mCopy.m_values[i], 1e-4f);
        }
    }


    //----------------------------------------------------------------------------------------------------------------------
    // Test 10: GetTranslation3D
    //
    TEST(Mat44Tests, GetTranslation3D)
    {
        Mat44 m = Mat44::CreateTranslation3D(7.f, 8.f, 9.f);
        Vec3 t = m.GetTranslation3D();
        EXPECT_FLOAT_EQ(t.x, 7.f);
        EXPECT_FLOAT_EQ(t.y, 8.f);
        EXPECT_FLOAT_EQ(t.z, 9.f);
    }


    //----------------------------------------------------------------------------------------------------------------------
    // Test 11: AppendXRotation, AppendYRotation, AppendZRotation
    //
    TEST(Mat44Tests, AppendRotations)
    {
        Mat44 m = Mat44::Identity;
        m.AppendXRotation(90.f);
        m.AppendYRotation(45.f);
        m.AppendZRotation(30.f);

        Mat44 expected = Mat44::Identity;
        expected.Append(Mat44::CreateXRotationDegrees(90.f));
        expected.Append(Mat44::CreateYRotationDegrees(45.f));
        expected.Append(Mat44::CreateZRotationDegrees(30.f));

        for (int i = 0; i < 16; ++i)
        {
            EXPECT_NEAR(m.m_values[i], expected.m_values[i], 1e-4f);
        }
    }


    //----------------------------------------------------------------------------------------------------------------------
    // Test 12: AppendUniformScale2D, AppendUniformScale3D
    //
    TEST(Mat44Tests, AppendUniformScale)
    {
        Mat44 m = Mat44::Identity;
        m.AppendUniformScale2D(2.f);
        m.AppendUniformScale3D(3.f);

        Mat44 expected = Mat44::Identity;
        expected.Append(Mat44::CreateUniformScale2D(2.f));
        expected.Append(Mat44::CreateUniformScale3D(3.f));

        for (int i = 0; i < 16; ++i)
        {
            EXPECT_NEAR(m.m_values[i], expected.m_values[i], 1e-4f);
        }
    }


    //----------------------------------------------------------------------------------------------------------------------
    // Test 13: AppendTranslation2D, AppendTranslation3D
    //
    TEST(Mat44Tests, AppendTranslation)
    {
        Mat44 m = Mat44::Identity;
        m.AppendTranslation2D(Vec2(1.f, 2.f));
        m.AppendTranslation3D(Vec3(3.f, 4.f, 5.f));

        Mat44 expected = Mat44::Identity;
        expected.Append(Mat44::CreateTranslation3D(1.f, 2.f, 0.f));
        expected.Append(Mat44::CreateTranslation3D(Vec3(3.f, 4.f, 5.f)));

        for (int i = 0; i < 16; ++i)
        {
            EXPECT_NEAR(m.m_values[i], expected.m_values[i], 1e-4f);
        }
    }


    //----------------------------------------------------------------------------------------------------------------------
    // Test 14: SetTranslation, SetTranslation2D, SetTranslation3D
    //
    TEST(Mat44Tests, SetTranslation)
    {
        Mat44 m;
        m.SetTranslation(1.f, 2.f, 3.f, 4.f);
        EXPECT_FLOAT_EQ(m.m_values[Mat44::Tx], 1.f);
        EXPECT_FLOAT_EQ(m.m_values[Mat44::Ty], 2.f);
        EXPECT_FLOAT_EQ(m.m_values[Mat44::Tz], 3.f);
        EXPECT_FLOAT_EQ(m.m_values[Mat44::Tw], 4.f);

        m.SetTranslation2D(Vec2(5.f, 6.f));
        EXPECT_FLOAT_EQ(m.m_values[Mat44::Tx], 5.f);
        EXPECT_FLOAT_EQ(m.m_values[Mat44::Ty], 6.f);
        EXPECT_FLOAT_EQ(m.m_values[Mat44::Tz], 0.f);
        EXPECT_FLOAT_EQ(m.m_values[Mat44::Tw], 1.f);

        m.SetTranslation3D(Vec3(7.f, 8.f, 9.f));
        EXPECT_FLOAT_EQ(m.m_values[Mat44::Tx], 7.f);
        EXPECT_FLOAT_EQ(m.m_values[Mat44::Ty], 8.f);
        EXPECT_FLOAT_EQ(m.m_values[Mat44::Tz], 9.f);
        EXPECT_FLOAT_EQ(m.m_values[Mat44::Tw], 1.f);
    }


    //----------------------------------------------------------------------------------------------------------------------
    // Test 15: SetIJK
    //
    TEST(Mat44Tests, SetIJK)
    {
        Mat44 m;
        Vec3 i(1.f, 2.f, 3.f);
        Vec3 j(4.f, 5.f, 6.f);
        Vec3 k(7.f, 8.f, 9.f);
        m.SetIJK(i, j, k);

        EXPECT_FLOAT_EQ(m.m_values[Mat44::Ix], 1.f);
        EXPECT_FLOAT_EQ(m.m_values[Mat44::Iy], 2.f);
        EXPECT_FLOAT_EQ(m.m_values[Mat44::Iz], 3.f);
        EXPECT_FLOAT_EQ(m.m_values[Mat44::Iw], 0.f);

        EXPECT_FLOAT_EQ(m.m_values[Mat44::Jx], 4.f);
        EXPECT_FLOAT_EQ(m.m_values[Mat44::Jy], 5.f);
        EXPECT_FLOAT_EQ(m.m_values[Mat44::Jz], 6.f);
        EXPECT_FLOAT_EQ(m.m_values[Mat44::Jw], 0.f);

        EXPECT_FLOAT_EQ(m.m_values[Mat44::Kx], 7.f);
        EXPECT_FLOAT_EQ(m.m_values[Mat44::Ky], 8.f);
        EXPECT_FLOAT_EQ(m.m_values[Mat44::Kz], 9.f);
        EXPECT_FLOAT_EQ(m.m_values[Mat44::Kw], 0.f);
    }


    //----------------------------------------------------------------------------------------------------------------------
    // Test 16: Operator==
    //
    TEST(Mat44Tests, OperatorEquals)
    {
        Mat44 a = Mat44::CreateTranslation3D(1.f, 2.f, 3.f);
        Mat44 b = Mat44::CreateTranslation3D(1.f, 2.f, 3.f);
        Mat44 c = Mat44::CreateTranslation3D(4.f, 5.f, 6.f);

        EXPECT_TRUE(a == b);
        EXPECT_FALSE(a == c);
    }


    //----------------------------------------------------------------------------------------------------------------------
    // Test 17: Static CreateTranslation3D
    //
    TEST(Mat44Tests, StaticCreateTranslation3D)
    {
        Mat44 m1 = Mat44::CreateTranslation3D(1.f, 2.f, 3.f);
        EXPECT_FLOAT_EQ(m1.m_values[Mat44::Tx], 1.f);
        EXPECT_FLOAT_EQ(m1.m_values[Mat44::Ty], 2.f);
        EXPECT_FLOAT_EQ(m1.m_values[Mat44::Tz], 3.f);

        Vec3 t(4.f, 5.f, 6.f);
        Mat44 m2 = Mat44::CreateTranslation3D(t);
        EXPECT_FLOAT_EQ(m2.m_values[Mat44::Tx], 4.f);
        EXPECT_FLOAT_EQ(m2.m_values[Mat44::Ty], 5.f);
        EXPECT_FLOAT_EQ(m2.m_values[Mat44::Tz], 6.f);
    }


    //----------------------------------------------------------------------------------------------------------------------
    // Test 18: Static CreateXRotationDegrees, CreateYRotationDegrees, CreateZRotationDegrees
    //
    TEST(Mat44Tests, StaticCreateRotationDegrees)
    {
        Mat44 mx = Mat44::CreateXRotationDegrees(90.f);
        Mat44 my = Mat44::CreateYRotationDegrees(90.f);
        Mat44 mz = Mat44::CreateZRotationDegrees(90.f);

        // Just check a few key values for correctness
        EXPECT_NEAR(mx.m_values[Mat44::Jy], 0.f, 1e-4f);
        EXPECT_NEAR(mx.m_values[Mat44::Jz], 1.f, 1e-4f);
        EXPECT_NEAR(mx.m_values[Mat44::Ky], -1.f, 1e-4f);
        EXPECT_NEAR(mx.m_values[Mat44::Kz], 0.f, 1e-4f);

        EXPECT_NEAR(my.m_values[Mat44::Ix], 0.f, 1e-4f);
        EXPECT_NEAR(my.m_values[Mat44::Iz], -1.f, 1e-4f);
        EXPECT_NEAR(my.m_values[Mat44::Kx], 1.f, 1e-4f);
        EXPECT_NEAR(my.m_values[Mat44::Kz], 0.f, 1e-4f);

        EXPECT_NEAR(mz.m_values[Mat44::Ix], 0.f, 1e-4f);
        EXPECT_NEAR(mz.m_values[Mat44::Iy], 1.f, 1e-4f);
        EXPECT_NEAR(mz.m_values[Mat44::Jx], -1.f, 1e-4f);
        EXPECT_NEAR(mz.m_values[Mat44::Jy], 0.f, 1e-4f);
    }


    //----------------------------------------------------------------------------------------------------------------------
    // Test 19: Static CreateUniformScale2D, CreateUniformScale3D
    //
    TEST(Mat44Tests, StaticCreateUniformScale)
    {
        Mat44 m2d = Mat44::CreateUniformScale2D(2.f);
        EXPECT_FLOAT_EQ(m2d.m_values[Mat44::Ix], 2.f);
        EXPECT_FLOAT_EQ(m2d.m_values[Mat44::Jy], 2.f);

        Mat44 m3d = Mat44::CreateUniformScale3D(3.f);
        EXPECT_FLOAT_EQ(m3d.m_values[Mat44::Ix], 3.f);
        EXPECT_FLOAT_EQ(m3d.m_values[Mat44::Jy], 3.f);
        EXPECT_FLOAT_EQ(m3d.m_values[Mat44::Kz], 3.f);
    }


    //----------------------------------------------------------------------------------------------------------------------
    // Test 20: Static CreateOrthoProjection
    //
    TEST(Mat44Tests, StaticCreateOrthoProjection)
    {
        float left = -1.f, right = 1.f, bottom = -1.f, top = 1.f, zNear = 0.f, zFar = 10.f;
        Mat44 ortho = Mat44::CreateOrthoProjection(left, right, bottom, top, zNear, zFar);

        EXPECT_FLOAT_EQ(ortho.m_values[Mat44::Ix], 1.f);
        EXPECT_FLOAT_EQ(ortho.m_values[Mat44::Jy], 1.f);
        EXPECT_FLOAT_EQ(ortho.m_values[Mat44::Kz], 0.1f);
        EXPECT_FLOAT_EQ(ortho.m_values[Mat44::Tx], 0.f);
        EXPECT_FLOAT_EQ(ortho.m_values[Mat44::Ty], 0.f);
        EXPECT_FLOAT_EQ(ortho.m_values[Mat44::Tz], 0.f);
    }

}
