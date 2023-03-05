// Bradley Christensen - 2022-2023
#include "EngineConstantBuffers.h"



bool CameraConstants::operator==(CameraConstants const& rhs) const
{
    return m_worldToCamera == rhs.m_worldToCamera && m_cameraToClip == rhs.m_cameraToClip;
}



bool CameraConstants::operator!=(CameraConstants const& rhs) const
{
    return !((*this) == rhs);
}



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



bool ModelConstants::operator!=(ModelConstants const& rhs) const
{
    return !((*this) == rhs);
}

