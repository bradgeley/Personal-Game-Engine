// Bradley Christensen - 2022-2026
#include "AbilityFlags.h"
#include "EntityDebugContext.h"
#include "Engine/Core/StringUtils.h"
#include "Engine/Core/XmlUtils.h"



//----------------------------------------------------------------------------------------------------------------------
AbilityFlags::AbilityFlags(void const* xmlElement)
{
	XmlElement const& elem = *reinterpret_cast<XmlElement const*>(xmlElement);

	auto const& flagNames = AbilityFlags::GetAllFlagNames();

	Name flags = XmlUtils::ParseXmlAttribute(elem, "flags", Name::Invalid);
	Strings flagList = StringUtils::SplitStringOnDelimiter(flags.ToString(), ',');

	for (size_t i = 0; i < flagList.size(); ++i)
	{
		Name const& flagName = Name(flagList[i]);
		for (size_t j = 0; j < flagNames.size(); ++j)
		{
			if (flagName == flagNames[j])
			{
				m_flags |= (1 << j);
				break;
			}
		}
	}
}



//----------------------------------------------------------------------------------------------------------------------
bool AbilityFlags::HasFlag(AbilityFlag flag) const
{
	return m_flags & (1 << static_cast<size_t>(flag));
}



//----------------------------------------------------------------------------------------------------------------------
void AbilityFlags::AppendDebugString(EntityDebugContext& debugContext) const
{
	auto const& flagNames = GetAllFlagNames();

	debugContext.m_debugString += StringUtils::StringF("---Ability Flags---\n");

	for (size_t i = 0; i < flagNames.size(); ++i)
	{
		if (HasFlag(static_cast<AbilityFlag>(i)))
		{
			debugContext.m_debugString += StringUtils::StringF("%s\n", flagNames[i].ToCStr());
		}
	}
}



//----------------------------------------------------------------------------------------------------------------------
std::array<Name, static_cast<size_t>(AbilityFlag::Count)> AbilityFlags::GetAllFlagNames()
{
	static std::array<Name, static_cast<size_t>(AbilityFlag::Count)> s_flagNames = { "Hit", "Splash", "Puddle" };
	//static_assert(s_flagNames.size() == static_cast<size_t>(AbilityFlag::Count), "AbilityFlags::GetAllFlagNames - s_flagNames size does not match AbilityFlag::Count."); // doesnt work?
	return s_flagNames;
}
