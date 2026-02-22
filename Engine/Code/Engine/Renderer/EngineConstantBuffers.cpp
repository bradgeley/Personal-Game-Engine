// Bradley Christensen - 2022-2026
#include "EngineConstantBuffers.h"
#include "Rgba8.h"



//----------------------------------------------------------------------------------------------------------------------
bool CameraConstants::operator==(CameraConstants const& rhs) const
{
    return m_worldToCamera == rhs.m_worldToCamera && m_cameraToClip == rhs.m_cameraToClip;
}



//----------------------------------------------------------------------------------------------------------------------
bool CameraConstants::operator!=(CameraConstants const& rhs) const
{
    return !((*this) == rhs);
}



//----------------------------------------------------------------------------------------------------------------------
bool ModelConstants::operator==(ModelConstants const& rhs) const
{
    for (int i = 0; i < 4; ++i)
    {
        if (m_modelRgba[i] != rhs.m_modelRgba[i])
        {
            return false;
        }
    }
    return m_modelMatrix == rhs.m_modelMatrix;
}



//----------------------------------------------------------------------------------------------------------------------
bool ModelConstants::operator!=(ModelConstants const& rhs) const
{
    return !((*this) == rhs);
}



//----------------------------------------------------------------------------------------------------------------------
FontConstants::FontConstants(Rgba8 const& tint, float boldness, float antiAlias, float outlineThickness)
    : m_boldness(boldness), m_antiAliasAmount(antiAlias), m_outlineThickness(outlineThickness)
{
    tint.GetAsFloats(m_outlineTint);
}



//----------------------------------------------------------------------------------------------------------------------
bool FontConstants::operator==(FontConstants const& rhs) const
{
    for (int i = 0; i < 4; ++i)
    {
        if (m_outlineTint[i] != rhs.m_outlineTint[i])
        {
            return false;
        }
    }
    if (m_boldness != rhs.m_boldness)
    {
        return false;
    }
    if (m_antiAliasAmount != rhs.m_antiAliasAmount)
    {
        return false;
    }
    if (m_outlineThickness != rhs.m_outlineThickness)
    {
        return false;
    }
    return true;
}



//----------------------------------------------------------------------------------------------------------------------
bool FontConstants::operator!=(FontConstants const& rhs) const
{
    return !((*this) == rhs);
}

