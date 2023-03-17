// Bradley Christensen - 2022-2023
#pragma once
#include "Engine/Math/Mat44.h"



struct Rgba8;



//----------------------------------------------------------------------------------------------------------------------
struct CameraConstants
{
    Mat44 m_worldToCamera   = Mat44();  // View Matrix
    Mat44 m_cameraToClip    = Mat44();  // Projection Matrix
    
    bool operator==(CameraConstants const& rhs) const;
    bool operator!=(CameraConstants const& rhs) const;
};



//----------------------------------------------------------------------------------------------------------------------
struct ModelConstants
{
    Mat44 m_modelMatrix     = Mat44();
    float m_modelRgba[4]    = { 1.f, 1.f, 1.f, 1.f };
    
    bool operator==(ModelConstants const& rhs) const;
    bool operator!=(ModelConstants const& rhs) const;
};



//----------------------------------------------------------------------------------------------------------------------
struct FontConstants
{
    FontConstants() = default;
    FontConstants(Rgba8 const& tint, float boldness, float antiAlias, float outlineThickness);
    
    float m_outlineTint[4]              = { 1.f, 1.f, 1.f, 1.f };
    float m_boldness                    = 0.5f;
    float m_antiAliasAmount             = 0.1f;
    float m_outlineThickness            = 0.f;
    float pad = -1;
    
    bool operator==(FontConstants const& rhs) const;
    bool operator!=(FontConstants const& rhs) const;
};