// Bradley Christensen - 2022-2026
#pragma once



struct Vec3;
struct Vec2;



//----------------------------------------------------------------------------------------------------------------------
// Mat44
//
// A 4x4 matrix! Simple and easy, right?
//
struct Mat44
{
    enum
    {
        Ix, Iy, Iz, Iw,   Jx, Jy, Jz, Jw,   Kx, Ky, Kz, Kw,   Tx, Ty, Tz, Tw
    };
    
    float m_values[16] = {};

public:
    
    Mat44();
    explicit Mat44(Vec2 const& iBasis2D, Vec2 const& jBasis2D, Vec2 const& translation2D);
    
    float* GetAsFloatArray();
    float const* GetAsFloatArray() const;
    float& operator[](int index);

    void Reset();
    void Append(Mat44 const& appendThis);
    Mat44 GetAppended(Mat44 const& appendThis) const;
    void Transpose();
    void InvertOrthoNormal();
    Mat44 GetOrthoNormalInverse() const;

    Vec3 GetTranslation3D() const;

    void AppendXRotation(float degreesRotationAboutX);
    void AppendYRotation(float degreesRotationAboutY);
    void AppendZRotation(float degreesRotationAboutZ);
    void AppendUniformScale2D(float uniformScale);
    void AppendUniformScale3D(float uniformScale);
    void AppendTranslation2D(Vec2 const& translation);
    void AppendTranslation3D(Vec3 const& translation);

    void SetTranslation(float x, float y = 0.f, float z = 0.f, float w = 1.f);
    void SetTranslation2D(Vec2 const& translationXY);
    void SetTranslation3D(Vec3 const& translationXYZ);
    void SetIJK(Vec3 const& i, Vec3 const& j, Vec3 const& k);

    bool operator==(Mat44 const& rhs) const;

public:

    static Mat44 CreateTranslation3D(float x, float y = 0.f, float z = 0.f);
    static Mat44 CreateTranslation3D(Vec3 const& translation);
    static Mat44 CreateXRotationDegrees(float rotationDegreesAboutX);
    static Mat44 CreateYRotationDegrees(float rotationDegreesAboutY);
    static Mat44 CreateZRotationDegrees(float rotationDegreesAboutZ);
    static Mat44 CreateUniformScale2D(float uniformScale);
    static Mat44 CreateUniformScale3D(float uniformScale);
    static Mat44 CreateOrthoProjection(float left, float right, float bottom, float top, float zNear, float zFar);
    
    static const Mat44 Identity;
};
