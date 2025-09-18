// Bradley Christensen - 2022-2025
#pragma once
#include <cstddef>
#include <vector>
#include <string>

// Todo enum to string macro?

//----------------------------------------------------------------------------------------------------------------------
enum class InputLayoutAttributeFormat
{
    Float1 = 0,
    Float2,
    Float3,
    Float4,
    Uint1,
    Uint2,
    Uint3,
    Uint4,
    Rgba8,

    Count,
    Invalid,
};



//----------------------------------------------------------------------------------------------------------------------
static std::string s_inputLayoutAttributeFormatNames[] =
{
    "Float1",
    "Float2",
    "Float3",
    "Float4",
    "Uint1",
    "Uint2",
    "Uint3",
    "Uint4",
    "Rgba8",

    "COUNT",
    "INVALID",
};



//----------------------------------------------------------------------------------------------------------------------
enum class InputLayoutSemantic
{
    Position = 0,
    Tint,
    Uvs,
    Normal,
    Tangent,
    InstancePosition,
    InstanceScale,
    InstanceRotation,
    InstanceTint,
    Index,

    Count,
    Invalid,
};



//----------------------------------------------------------------------------------------------------------------------
static std::string s_inputLayoutSemanticNames[] =
{
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

    "COUNT",
    "INVALID",
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

    static std::string GetInputLayoutSemanticName(InputLayoutSemantic semantic);
	static InputLayoutSemantic GetInputLayoutSemanticFromString(std::string const& str);
	static InputLayoutAttributeFormat GetInputLayoutAttributeFormatFromString(std::string const& str);
	static InputLayout Combine(InputLayout const& a, InputLayout const& b);
	static InputLayout Combine(InputLayout const** ppData, size_t numLayouts);

public:

	std::vector<InputLayoutAttribute> m_attributes;
};