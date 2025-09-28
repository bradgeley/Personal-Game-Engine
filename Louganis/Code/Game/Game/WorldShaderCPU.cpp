// Bradley Christensen - 2022-2025
#include "WorldShaderCPU.h"
#include "Engine/Renderer/InputLayout.h"



//----------------------------------------------------------------------------------------------------------------------
TerrainVertex::TerrainVertex(Vec3 const& pos, Rgba8 const& tint, Vec2 const& uvs, Vec2 const& lightmapUVs)
    : m_pos(pos)
    , m_tint(tint)
    , m_uvs(uvs)
    , m_lightmapUVs(lightmapUVs)
{
}



//----------------------------------------------------------------------------------------------------------------------
InputLayout* TerrainVertex::GetInputLayout()
{
    static InputLayout s_inputLayout;

    if (s_inputLayout.m_attributes.empty())
    {
        s_inputLayout.m_attributes.reserve(3);

        s_inputLayout.m_attributes.push_back(InputLayoutAttribute
        {
            InputLayoutSemantic::Position,                      // Semantic: POSITION
            0,                                                  // Semantic index
            InputLayoutAttributeFormat::Float3,                 // Format
            0,                                                  // Input slot
            (uint32_t) offsetof(TerrainVertex, m_pos),          // Offset in struct
            false                                               // Per-instance
        });

        s_inputLayout.m_attributes.push_back(InputLayoutAttribute
        {
            InputLayoutSemantic::Tint,                          // Semantic: TINT
            0,                                                  // Semantic index
            InputLayoutAttributeFormat::Rgba8,                  // Format
            0,                                                  // Input slot
            (uint32_t) offsetof(TerrainVertex, m_tint),         // Offset in struct
            false                                               // Per-instance
        });

        s_inputLayout.m_attributes.push_back(InputLayoutAttribute
        {
            InputLayoutSemantic::Uvs,                           // Semantic: UVS
            0,                                                  // Semantic index
            InputLayoutAttributeFormat::Float2,                 // Format
            0,                                                  // Input slot
            (uint32_t) offsetof(TerrainVertex, m_uvs),          // Offset in struct
            false                                               // Per-instance
        });

        s_inputLayout.m_attributes.push_back(InputLayoutAttribute
        {
            InputLayoutSemantic::LightmapUVs,                   // Semantic: LIGHTMAPUVS
            0,                                                  // Semantic index
            InputLayoutAttributeFormat::Float2,                 // Format
            0,                                                  // Input slot
            (uint32_t) offsetof(TerrainVertex, m_lightmapUVs),  // Offset in struct
            false                                               // Per-instance
        });
    }

    return &s_inputLayout;
}