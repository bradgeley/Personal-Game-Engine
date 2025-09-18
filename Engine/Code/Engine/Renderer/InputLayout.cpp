// Bradley Christensen - 2022-2025
#include "InputLayout.h"



//----------------------------------------------------------------------------------------------------------------------
const char* InputLayout::GetInputLayoutSemanticName(InputLayoutSemantic semantic)
{
    return s_inputLayoutSemanticNames[(int) semantic];
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