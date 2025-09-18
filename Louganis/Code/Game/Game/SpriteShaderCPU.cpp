// Bradley Christensen - 2022-2025
#include "SpriteShaderCPU.h"
#include "Engine/Renderer/InputLayout.h"



//----------------------------------------------------------------------------------------------------------------------
InputLayout* SpriteInstance::GetInputLayout()
{
    static InputLayout s_spriteInstanceInputLayout;

    if (s_spriteInstanceInputLayout.m_attributes.empty())
    {
        s_spriteInstanceInputLayout.m_attributes.reserve(3);

        s_spriteInstanceInputLayout.m_attributes.push_back(InputLayoutAttribute
        {
            InputLayoutSemantic::InstancePosition,              // Semantic: INSTANCEPOSITION
            0,                                                  // Semantic index
            InputLayoutAttributeFormat::Float3,                 // Format
            1,                                                  // Input slot
            (uint32_t) offsetof(SpriteInstance, m_position),    // Offset in struct
            true                                                // Per-instance
        });

        s_spriteInstanceInputLayout.m_attributes.push_back(InputLayoutAttribute
        {
            InputLayoutSemantic::InstanceRotation,              // Semantic: INSTANCEROTATION
            0,                                                  // Semantic index
            InputLayoutAttributeFormat::Float1,                 // Format
            1,                                                  // Input slot
            (uint32_t) offsetof(SpriteInstance, m_orientation), // Offset in struct
            true                                                // Per-instance
        });

        s_spriteInstanceInputLayout.m_attributes.push_back(InputLayoutAttribute
        {
            InputLayoutSemantic::InstanceScale,                 // Semantic: INSTANCESCALE
            0,                                                  // Semantic index
            InputLayoutAttributeFormat::Float1,                 // Format
            1,                                                  // Input slot
            (uint32_t) offsetof(SpriteInstance, m_scale),       // Offset in struct
            true                                                // Per-instance
        });
        s_spriteInstanceInputLayout.m_attributes.push_back(InputLayoutAttribute
        {
			InputLayoutSemantic::InstanceTint,                  // Semantic: INSTANCETINT
            0,                                                  // Semantic index
            InputLayoutAttributeFormat::Rgba8,                  // Format
            1,                                                  // Input slot
            (uint32_t) offsetof(SpriteInstance, m_rgba),        // Offset in struct
            true                                                // Per-instance
        });
        s_spriteInstanceInputLayout.m_attributes.push_back(InputLayoutAttribute
        {
            InputLayoutSemantic::Index,                         // Semantic: INDEX
            0,                                                  // Semantic index
            InputLayoutAttributeFormat::Uint1,                  // Format
            1,                                                  // Input slot
            (uint32_t) offsetof(SpriteInstance, m_spriteIndex), // Offset in struct
            true                                                // Per-instance
        });
    }

    return &s_spriteInstanceInputLayout;
}