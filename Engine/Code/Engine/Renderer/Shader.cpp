// Bradley Christensen - 2022-2025
#include "Engine/Renderer/Shader.h"
#include "Engine/Core/XmlUtils.h"
#include "Engine/Core/ErrorUtils.h"
#include "Engine/Core/StringUtils.h"



//----------------------------------------------------------------------------------------------------------------------
Shader::Shader(ShaderConfig const& config) : m_config(config)
{
	
}



//----------------------------------------------------------------------------------------------------------------------
ShaderConfig ShaderConfig::MakeFromXML(char const* xmlFilePath)
{
	ShaderConfig result;

	XmlDocument doc;
	XmlError succeeded = doc.LoadFile(xmlFilePath);
	ASSERT_OR_DIE(succeeded == tinyxml2::XML_SUCCESS, StringUtils::StringF("Failed to load ShaderConfig XML file %s", xmlFilePath));
	XmlElement* element = doc.RootElement();
	ASSERT_OR_DIE(element, StringUtils::StringF("ShaderConfig XML file %s has no root element", xmlFilePath));
	result.m_name = XmlUtils::ParseXmlAttribute(*element, "name", Name::Invalid);
	result.m_sourceFilePath = XmlUtils::ParseXmlAttribute(*element, "source", "");
	result.m_pixelEntry = XmlUtils::ParseXmlAttribute(*element, "pixel", "");
	result.m_vertexEntry = XmlUtils::ParseXmlAttribute(*element, "vertex", "");

	XmlElement const* inputLayoutElement = element->FirstChildElement("InputLayout");
	if (inputLayoutElement)
	{
		XmlElement const* attributeElement = inputLayoutElement->FirstChildElement("InputLayoutAttribute");
		while (attributeElement)
		{
			InputLayoutAttribute attribute;
			std::string inputLayoutSemanticString = XmlUtils::ParseXmlAttribute(*attributeElement, "semantic", "");
			attribute.m_semantic = InputLayout::GetInputLayoutSemanticFromString(inputLayoutSemanticString);
			attribute.m_semanticIndex = (uint32_t) XmlUtils::ParseXmlAttribute(*attributeElement, "semanticIndex", 0);
			std::string formatString = XmlUtils::ParseXmlAttribute(*attributeElement, "format", "");
			attribute.m_format = InputLayout::GetInputLayoutAttributeFormatFromString(formatString);
			attribute.m_inputSlot = (uint32_t) XmlUtils::ParseXmlAttribute(*attributeElement, "inputSlot", 0);
			attribute.m_alignedByteOffset = (uint32_t) XmlUtils::ParseXmlAttribute(*attributeElement, "alignedByteOffset", 0);
			attribute.m_isPerInstance = XmlUtils::ParseXmlAttribute(*attributeElement, "isPerInstance", false);

			result.m_inputLayout.m_attributes.push_back(attribute);
			attributeElement = attributeElement->NextSiblingElement("InputLayoutAttribute");
		}
	}

	return result;
}
