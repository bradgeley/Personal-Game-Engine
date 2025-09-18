// Bradley Christensen - 2022-2025
#include "Engine/Renderer/Vertex_PCU.h"
#include "InputLayout.h"



//----------------------------------------------------------------------------------------------------------------------
Vertex_PCU::Vertex_PCU(Vec2 const& position, Rgba8 const& tint, Vec2 const& uvs)
    : pos(Vec3(position)), tint(tint), uvs(uvs)
{

}



//----------------------------------------------------------------------------------------------------------------------
Vertex_PCU::Vertex_PCU(Vec3 const& position, Rgba8 const& tint, Vec2 const& uvs)
    : pos(position), tint(tint), uvs(uvs)
{

}



//----------------------------------------------------------------------------------------------------------------------
InputLayout* Vertex_PCU::GetInputLayout()
{
    static InputLayout s_vertexPcuInputLayout;

    if (s_vertexPcuInputLayout.m_attributes.empty())
    {
        s_vertexPcuInputLayout.m_attributes.reserve(3);

        s_vertexPcuInputLayout.m_attributes.push_back(InputLayoutAttribute
        {
            InputLayoutSemantic::Position,                      // Semantic: POSITION
            0,                                                  // Semantic index
            InputLayoutAttributeFormat::Float3,                 // Format: 3 floats
            0,                                                  // Input slot
            static_cast<uint32_t>(offsetof(Vertex_PCU, pos)),   // Offset in struct
            false                                               // Per-instance
        });

        s_vertexPcuInputLayout.m_attributes.push_back(InputLayoutAttribute
        {
            InputLayoutSemantic::Tint,                          // Semantic: TINT
            0,                                                  // Semantic index
            InputLayoutAttributeFormat::Rgba8,                  // Format: packed RGBA8
            0,                                                  // Input slot
            static_cast<uint32_t>(offsetof(Vertex_PCU, tint)),  // Offset in struct
            false                                               // Per-instance
        });

        s_vertexPcuInputLayout.m_attributes.push_back(InputLayoutAttribute
        {
            InputLayoutSemantic::Uvs,                           // Semantic: UVS
            0,                                                  // Semantic index
            InputLayoutAttributeFormat::Float2,                 // Format: 2 floats
            0,                                                  // Input slot
            static_cast<uint32_t>(offsetof(Vertex_PCU, uvs)),   // Offset in struct
            false                                               // Per-instance
        });
    }

	return &s_vertexPcuInputLayout;
}
