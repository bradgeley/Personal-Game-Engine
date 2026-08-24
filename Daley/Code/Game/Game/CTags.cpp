// Bradley Christensen - 2022-2026
#include "CTags.h"
#include "Engine/Core/StringUtils.h"
#include "Engine/Core/XmlUtils.h"
#include "Engine/Debug/DevConsoleUtils.h"



//----------------------------------------------------------------------------------------------------------------------
CTags::CTags(void const* xmlElement)
{
	XmlElement const& elem = *reinterpret_cast<XmlElement const*>(xmlElement);

	XmlAttribute const* attribute = elem.FindAttribute("tags");
	if (attribute)
	{
		std::string csv = attribute->Value();
		Strings tags = StringUtils::SplitStringOnDelimiter(csv, ',');

		for (std::string& tag : tags)
		{
			AddTag(Name(tag));
		}
	}
}



//----------------------------------------------------------------------------------------------------------------------
bool CTags::AddTag(Name const& tag)
{
	if (HasTag(tag))
	{
		return true;
	}

	int emptyIndex = FindTag(Name::Invalid);
	if (emptyIndex == -1)
	{
		DevConsoleUtils::LogError("Cannot add tag '%s': no available slot for new tag", tag.ToCStr());
		return false;
	}

	m_tags[emptyIndex] = tag;
	return true;
}



//----------------------------------------------------------------------------------------------------------------------
bool CTags::RemoveTag(Name const& tag)
{
	for (int i = 0; i < MAX_TAGS; ++i)
	{
		if (m_tags[i] == tag)
		{
			m_tags[i] = Name::Invalid;
			return true;
		}
	}
	return false;
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



//----------------------------------------------------------------------------------------------------------------------
void CTags::AppendDebugString(std::string& out) const
{
	bool hasTag = false;
	for (int i = 0; i < MAX_TAGS; ++i)
	{
		if (m_tags[i] != Name::Invalid)
		{
			if (!hasTag)
			{
				out += "Tags: ";
				hasTag = true;
			}
			out += m_tags[i].ToCStr();
			out += " ";
		}
	}
	if (hasTag)
	{
		out += "\n";
	}
}
