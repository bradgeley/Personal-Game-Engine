// Bradley Christensen - 2023
#pragma once



struct Vec2;



//----------------------------------------------------------------------------------------------------------------------
// Mat44
//
// A 4x4 matrix! Simple and easy
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

    void Append(Mat44 const& appendThis);
    
    float const* GetAsFloatArray() const;

public:
    
    static Mat44 CreateOrthoProjection(float left, float right, float bottom, float top, float zNear, float zFar);
    
    static const Mat44 Identity;
};
