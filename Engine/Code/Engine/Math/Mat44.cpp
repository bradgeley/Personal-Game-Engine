// Bradley Christensen - 2023
#include "Mat44.h"
#include <cstring>
#include "Vec2.h"



const Mat44 Mat44::Identity = Mat44();



Mat44::Mat44() : m_values{ 1, 0, 0, 0,  
                           0, 1, 0, 0,  
                           0, 0, 1, 0,  
                           0, 0, 0, 1 }
{
    
}



Mat44::Mat44(Vec2 const& iBasis2D, Vec2 const& jBasis2D, Vec2 const& translation2D)
    : m_values { iBasis2D.x,        iBasis2D.y,         0.f,    0.f,
                 jBasis2D.x,        jBasis2D.y,         0.f,    0.f,
                 0.f,               0.f,                1.f,    0.f,
                 translation2D.x,   translation2D.y,    0.f,    1.f }
{
    
}



void Mat44::Append(Mat44 const& appendThis)
{
    float const* append = appendThis.GetAsFloatArray();
	float result[16];

	result[Ix] = m_values[Ix] * append[Ix] + m_values[Jx] * append[Iy] + m_values[Kx] * append[Iz] + m_values[Tx] * append[Iw];
	result[Iy] = m_values[Iy] * append[Ix] + m_values[Jy] * append[Iy] + m_values[Ky] * append[Iz] + m_values[Ty] * append[Iw];
	result[Iz] = m_values[Iz] * append[Ix] + m_values[Jz] * append[Iy] + m_values[Kz] * append[Iz] + m_values[Tz] * append[Iw];
	result[Iw] = m_values[Iw] * append[Ix] + m_values[Jw] * append[Iy] + m_values[Kw] * append[Iz] + m_values[Tw] * append[Iw];

	result[Jx] = m_values[Ix] * append[Jx] + m_values[Jx] * append[Jy] + m_values[Kx] * append[Jz] + m_values[Tx] * append[Jw];
	result[Jy] = m_values[Iy] * append[Jx] + m_values[Jy] * append[Jy] + m_values[Ky] * append[Jz] + m_values[Ty] * append[Jw];
	result[Jz] = m_values[Iz] * append[Jx] + m_values[Jz] * append[Jy] + m_values[Kz] * append[Jz] + m_values[Tz] * append[Jw];
	result[Jw] = m_values[Iw] * append[Jx] + m_values[Jw] * append[Jy] + m_values[Kw] * append[Jz] + m_values[Tw] * append[Jw];

	result[Kx] = m_values[Ix] * append[Kx] + m_values[Jx] * append[Ky] + m_values[Kx] * append[Kz] + m_values[Tx] * append[Kw];
	result[Ky] = m_values[Iy] * append[Kx] + m_values[Jy] * append[Ky] + m_values[Ky] * append[Kz] + m_values[Ty] * append[Kw];
	result[Kz] = m_values[Iz] * append[Kx] + m_values[Jz] * append[Ky] + m_values[Kz] * append[Kz] + m_values[Tz] * append[Kw];
	result[Kw] = m_values[Iw] * append[Kx] + m_values[Jw] * append[Ky] + m_values[Kw] * append[Kz] + m_values[Tw] * append[Kw];

	result[Tx] = m_values[Ix] * append[Tx] + m_values[Jx] * append[Ty] + m_values[Kx] * append[Tz] + m_values[Tx] * append[Tw];
	result[Ty] = m_values[Iy] * append[Tx] + m_values[Jy] * append[Ty] + m_values[Ky] * append[Tz] + m_values[Ty] * append[Tw];
	result[Tz] = m_values[Iz] * append[Tx] + m_values[Jz] * append[Ty] + m_values[Kz] * append[Tz] + m_values[Tz] * append[Tw];
	result[Tw] = m_values[Iw] * append[Tx] + m_values[Jw] * append[Ty] + m_values[Kw] * append[Tz] + m_values[Tw] * append[Tw];

	memcpy(m_values, result, 16 * sizeof(float));
}



float const* Mat44::GetAsFloatArray() const
{
	return m_values;
}



Mat44 Mat44::CreateOrthoProjection(float left, float right, float bottom, float top, float zNear, float zFar)
{
	Mat44 result;
	
	float oneOverWidth  =  1.f / (right - left);
	float oneOverHeight =  1.f / (top - bottom);
	float oneOverDepth  = -1.f / (zFar - zNear);
	
	result.m_values[Ix]	= 2.f * oneOverWidth;
	result.m_values[Jy]	= 2.f * oneOverHeight;
	result.m_values[Kz]	= -1.f * oneOverDepth;

	result.m_values[Tx]	= -1.f * (right + left) * oneOverWidth;
	result.m_values[Ty]	= -1.f * (bottom + top) * oneOverHeight;
	result.m_values[Tz]	=  1.f * (zNear) * oneOverDepth;
	return result;
}
