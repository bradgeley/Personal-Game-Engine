// Bradley Christensen - 2022-2025
#include "InputLayout.h"
#include "Engine/Core/ErrorUtils.h"
#include "Engine/Core/StringUtils.h"



//----------------------------------------------------------------------------------------------------------------------
std::string InputLayout::GetInputLayoutSemanticName(InputLayoutSemantic semantic)
{
    return s_inputLayoutSemanticNames[(int) semantic];
}



//----------------------------------------------------------------------------------------------------------------------
InputLayoutSemantic InputLayout::GetInputLayoutSemanticFromString(std::string const& str)
{
    StringUtils::CaseInsensitiveStringEquals comparator;
    for (int i = 0; i < (int) InputLayoutSemantic::Count; ++i)
    {
        if (comparator(str, s_inputLayoutSemanticNames[i]))
        {
            return (InputLayoutSemantic) i;
		}
    }
    ERROR_AND_DIE(StringUtils::StringF("Invalid input layout semantic string: %s", str.c_str()));
    //return InputLayoutSemantic::Invalid;
}



//----------------------------------------------------------------------------------------------------------------------
InputLayoutAttributeFormat InputLayout::GetInputLayoutAttributeFormatFromString(std::string const& str)
{
    StringUtils::CaseInsensitiveStringEquals comparator;
    for (int i = 0; i < (int) InputLayoutAttributeFormat::Count; ++i)
    {
        if (comparator(str, s_inputLayoutAttributeFormatNames[i]))
        {
            return (InputLayoutAttributeFormat) i;
        }
    }
    ERROR_AND_DIE(StringUtils::StringF("Invalid input layout semantic string: %s", str.c_str()));
    // return InputLayoutAttributeFormat::Invalid;
}



//----------------------------------------------------------------------------------------------------------------------
InputLayout InputLayout::Combine(InputLayout const& a, InputLayout const& b)
{
	InputLayout const* layouts[2] = { &a, &b };
	return Combine(layouts, 2);
}



//----------------------------------------------------------------------------------------------------------------------
InputLayout InputLayout::Combine(InputLayout const** ppData, size_t numLayouts)
{
    InputLayout result;
    for (int i = 0; i < (int) numLayouts; ++i)
    {
        for (int attributeIndex = 0; attributeIndex < (int) ppData[i]->m_attributes.size(); ++attributeIndex)
        {
			InputLayoutAttribute attribute = ppData[i]->m_attributes[attributeIndex];
            attribute.m_inputSlot = (uint32_t) i;
			result.m_attributes.push_back(attribute);
        }
	}
    return result;
}



//----------------------------------------------------------------------------------------------------------------------
int InputLayout::GetNumBytesForFormat(InputLayoutAttributeFormat format)
{
    switch (format)
    {
		case InputLayoutAttributeFormat::Float1:    return 4;
		case InputLayoutAttributeFormat::Float2:    return 8;
		case InputLayoutAttributeFormat::Float3:    return 12;
		case InputLayoutAttributeFormat::Float4:    return 16;
		case InputLayoutAttributeFormat::Uint1:     return 4;
		case InputLayoutAttributeFormat::Uint2:	    return 8;
		case InputLayoutAttributeFormat::Uint3:	    return 12;
        case InputLayoutAttributeFormat::Uint4:     return 16;
		case InputLayoutAttributeFormat::Rgba8:     return 4;
    }
    return 0;
}
