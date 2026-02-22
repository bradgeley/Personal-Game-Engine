// Bradley Christensen - 2022-2025
#include "CTags.h"
#include "Engine/Core/XmlUtils.h"
#include "Engine/Debug/DevConsoleUtils.h"



//----------------------------------------------------------------------------------------------------------------------
CTags::CTags(void const* xmlElement)
{
	XmlElement const& elem = *reinterpret_cast<XmlElement const*>(xmlElement);

	XmlAttribute const* attribute = elem.FirstAttribute();
	while (attribute)
	{
		std::string name = attribute->Name();
		AddTag(Name(name));

		attribute = attribute->Next();
	}
}



//----------------------------------------------------------------------------------------------------------------------
void CTags::AddTag(Name const& tag)
{
	if (HasTag(tag))
	{
		return;
	}

	int emptyIndex = FindTag(Name::Invalid);
	if (emptyIndex == -1)
	{
		DevConsoleUtils::LogError("Cannot add tag '%s': no available slot for new tag", tag.ToCStr());
		return;
	}

	m_tags[emptyIndex] = tag;
}



//----------------------------------------------------------------------------------------------------------------------
void CTags::RemoveTag(Name const& tag)
{
	for (int i = 0; i < MAX_TAGS; ++i)
	{
		if (m_tags[i] == tag)
		{
			m_tags[i] = Name::Invalid;
			return;
		}
	}
}



//----------------------------------------------------------------------------------------------------------------------
bool CTags::HasTag(Name const& tag) const
{
	for (int i = 0; i < MAX_TAGS; ++i)
	{
		if (m_tags[i] == tag)
		{
			return true;
		}
	}
	return false;
}



//----------------------------------------------------------------------------------------------------------------------
int CTags::FindTag(Name const& tag) const
{
	for (int i = 0; i < MAX_TAGS; ++i)
	{
		if (m_tags[i] == tag)
		{
			return i;
		}
	}
	return -1;
}
