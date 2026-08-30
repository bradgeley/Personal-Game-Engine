// Bradley Christensen - 2022-2026
#include "RunModifierPool.h"
#include "Engine/Core/ErrorUtils.h"
#include "Engine/Core/StringUtils.h"
#include "Engine/Core/XmlUtils.h"



//----------------------------------------------------------------------------------------------------------------------
void RunModifierPool::LoadFromXml(const char* filePath)
{
	XmlDocument doc;
	doc.LoadFile(filePath);
	ASSERT_OR_DIE(doc.ErrorID() == tinyxml2::XML_SUCCESS, StringUtils::StringF("Failed to load RunModifierPool XML file \"%s\". Error: %s", filePath, doc.ErrorName()));

	XmlElement const* root = doc.RootElement();

	XmlElement const* modifierPoolElement = root->FirstChildElement("RunModifierPool");

	while (modifierPoolElement != nullptr)
	{
		XmlElement const* modifierElement = modifierPoolElement->FirstChildElement();
		while (modifierElement)
		{
			RunModifierDef const* modifierDef = RunModifierDef::MakeFromXml(*modifierElement);
			if (modifierDef)
			{
				m_runModifierDefs.push_back(modifierDef);
			}

			modifierElement = modifierElement->NextSiblingElement();
		}

		modifierPoolElement = modifierPoolElement->NextSiblingElement("RunModifierPool");
	}
}



//----------------------------------------------------------------------------------------------------------------------
void RunModifierPool::Shutdown()
{
	for (RunModifierDef const* modifierDef : m_runModifierDefs)
	{
		delete modifierDef;
	}
	m_runModifierDefs.clear();
}