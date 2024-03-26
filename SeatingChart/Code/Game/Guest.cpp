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
void Guest::AddFriend(std::string const& person)
{
	for (auto& otherGuest : m_knownOtherGuests)
	{
		if (otherGuest == person)
		{
			return;
		}
	}
	m_knownOtherGuests.push_back(person);
}



//----------------------------------------------------------------------------------------------------------------------
std::string Guest::ToString() const
{
	std::string result = StringF("%s: ", m_name.c_str());

	for (int i = 0; i < m_knownOtherGuests.size(); ++i)
	{
		auto& otherGuest = m_knownOtherGuests[i];
		result.append(otherGuest);

		if (i != 0 && i < m_knownOtherGuests.size() - 1)
		{
			result.append(", ");
		}
	}

	return result;
}



//----------------------------------------------------------------------------------------------------------------------
void Guest::ImportFromString(std::string const& string)
{
	Strings nameToGuestListPair = SplitStringOnDelimeter(string, ':');
	ASSERT_OR_DIE(nameToGuestListPair.size() >= 1, "nameToGuestListPair size was 0");

	m_name = nameToGuestListPair[0];
	TrimEdgeWhitespace(m_name);

	if (nameToGuestListPair.size() <= 1)
	{
		return;
	}

	std::string& otherGuestsCSV = nameToGuestListPair[1];
	Strings otherGuests = SplitStringOnDelimeter(otherGuestsCSV, ',');
	for (auto& otherGuest : otherGuests)
	{
		TrimEdgeWhitespace(otherGuest);
		m_knownOtherGuests.emplace_back(otherGuest);
	}
}
