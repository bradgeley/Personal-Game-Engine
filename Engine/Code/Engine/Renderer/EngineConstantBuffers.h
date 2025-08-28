// Bradley Christensen - 2022-2025
#pragma once
#include "Engine/Math/Mat44.h"



struct Rgba8;



//----------------------------------------------------------------------------------------------------------------------
struct alignas(16) CameraConstants
{
    Mat44 m_gameToRender    = Mat44();  // Game to Render Matrix (Simple axis transpose so user can define game axes)
    Mat44 m_worldToCamera   = Mat44();  // View Matrix
    Mat44 m_cameraToClip    = Mat44();  // Projection Matrix
    
    bool operator==(CameraConstants const& rhs) const;
    bool operator!=(CameraConstants const& rhs) const;
};



//----------------------------------------------------------------------------------------------------------------------
struct alignas(16) ModelConstants
{
    Mat44 m_modelMatrix     = Mat44();
    float m_modelRgba[4]    = { 1.f, 1.f, 1.f, 1.f };
    
    bool operator==(ModelConstants const& rhs) const;
    bool operator!=(ModelConstants const& rhs) const;
};



//----------------------------------------------------------------------------------------------------------------------
struct alignas(16) FontConstants
{
    FontConstants() = default;
    FontConstants(Rgba8 const& tint, float boldness, float antiAlias, float outlineThickness);
    
    float m_outlineTint[4]              = { 0.f, 0.f, 0.f, 0.f };
    float m_boldness                    = 0.66f;
    float m_antiAliasAmount             = 0.5f;
    float m_outlineThickness            = 0.f;

    float pad = 0.f;
    
    bool operator==(FontConstants const& rhs) const;
    bool operator!=(FontConstants const& rhs) const;
};