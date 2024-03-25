// Bradley Christensen - 2022-2024
#include "Guest.h"
#include "Engine/Core/StringUtils.h"
#include "Engine/Core/ErrorUtils.h"



//----------------------------------------------------------------------------------------------------------------------
std::string UNNAMED_GUEST = "Unnamed Guest";



//----------------------------------------------------------------------------------------------------------------------
bool Guest::IsValid() const
{
	return false;
}



//----------------------------------------------------------------------------------------------------------------------
std::string Guest::ToString() const
{
	std::string result = StringF("%s:", m_name.c_str());

	for (int i = 0; i < m_knownOtherGuests.size(); ++i)
	{
		auto& otherGuest = m_knownOtherGuests[i];
		result.append(otherGuest);

		if (i < m_knownOtherGuests.size() - 1)
		{
			result.append(",");
		}
	}

	return result;
}



//----------------------------------------------------------------------------------------------------------------------
void Guest::ImportFromString(std::string const& string)
{
	Strings nameToGuestListPair = SplitStringOnDelimeter(string, ':');
	ASSERT_OR_DIE(nameToGuestListPair.size() == 2, "nameToGuestListPair size was not 2");

	m_name = nameToGuestListPair[0];
	TrimWhitespace(m_name);

	std::string& otherGuestsCSV = nameToGuestListPair[1];
	Strings otherGuests = SplitStringOnDelimeter(otherGuestsCSV, ',');
	for (auto& otherGuest : otherGuests)
	{
		TrimWhitespace(otherGuest);
		m_knownOtherGuests.emplace_back(otherGuest);
	}
}
