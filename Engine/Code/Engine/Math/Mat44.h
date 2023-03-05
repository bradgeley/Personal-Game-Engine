// Bradley Christensen - 2022-2023
#pragma once



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

    void Append(Mat44 const& appendThis);
    Mat44 GetAppended(Mat44 const& appendThis) const;
    void AppendXRotation(float degreesRotationAboutX);
    void AppendYRotation(float degreesRotationAboutY);
    void AppendZRotation(float degreesRotationAboutZ);
    void AppendUniformScale2D(float uniformScale);
    void AppendUniformScale3D(float uniformScale);

    bool operator==(Mat44 const& rhs) const;

public:
    
    static Mat44 CreateXRotationDegrees(float rotationDegreesAboutX);
    static Mat44 CreateYRotationDegrees(float rotationDegreesAboutY);
    static Mat44 CreateZRotationDegrees(float rotationDegreesAboutZ);
    static Mat44 CreateUniformScale2D(float uniformScale);
    static Mat44 CreateUniformScale3D(float uniformScale);
    static Mat44 CreateOrthoProjection(float left, float right, float bottom, float top, float zNear, float zFar);
    
    static const Mat44 Identity;
};
