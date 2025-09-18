// Bradley Christensen - 2022-2025
#pragma once
#include <cstddef>
#include <vector>



//----------------------------------------------------------------------------------------------------------------------
enum class InputLayoutAttributeFormat
{
    Invalid,

    Float1,
    Float2,
    Float3,
    Float4,
    Uint1,
    Uint2,
    Uint3,
    Uint4,
    Rgba8,
};



//----------------------------------------------------------------------------------------------------------------------
enum class InputLayoutSemantic
{
    Invalid,

    Position,
    Tint,
    Uvs,
    Normal,
    Tangent,
    InstancePosition,
    InstanceScale,
    InstanceRotation,
    InstanceTint,
    Index,
};



//----------------------------------------------------------------------------------------------------------------------
static const char* s_inputLayoutSemanticNames[] =
{
    "INVALID",

    "POSITION",
    "TINT",
    "UVS",
    "NORMAL",
    "TANGENT",
    "INSTANCEPOSITION",
    "INSTANCESCALE",
    "INSTANCEROTATION",
    "INSTANCETINT",
	"INDEX",
};



//----------------------------------------------------------------------------------------------------------------------
struct InputLayoutAttribute
{
	InputLayoutSemantic         m_semantic          = InputLayoutSemantic::Invalid;
	uint32_t 	                m_semanticIndex     = 0;
	InputLayoutAttributeFormat  m_format            = InputLayoutAttributeFormat::Invalid;
	uint32_t                    m_inputSlot         = 0;
	uint32_t                    m_alignedByteOffset = 0;
	bool                        m_isPerInstance     = false;
};



//----------------------------------------------------------------------------------------------------------------------
// Input Layout
//
struct InputLayout
{
public:

    static const char* GetInputLayoutSemanticName(InputLayoutSemantic semantic);
	static InputLayout Combine(InputLayout const& a, InputLayout const& b);
	static InputLayout Combine(InputLayout const** ppData, size_t numLayouts);

public:

	std::vector<InputLayoutAttribute> m_attributes;
};