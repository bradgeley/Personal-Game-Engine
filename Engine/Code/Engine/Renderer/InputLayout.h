// Bradley Christensen - 2022-2026
#pragma once
#include "Engine/Core/Name.h"
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
    R8_UNORM,

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
	"Rgba8", // R8G8B8A8_UNORM
	"R8_UNORM",

    "COUNT",
    "INVALID",
};



//----------------------------------------------------------------------------------------------------------------------
struct InputLayoutAttribute
{
	Name                        m_semantic;
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

	static InputLayoutAttributeFormat GetInputLayoutAttributeFormatFromString(std::string const& str);
	static InputLayout Combine(InputLayout const& a, InputLayout const& b);
	static InputLayout Combine(InputLayout const** ppData, size_t numLayouts);
	static int GetNumBytesForFormat(InputLayoutAttributeFormat format);

public:

	std::vector<InputLayoutAttribute> m_attributes;
};