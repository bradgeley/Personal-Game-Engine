// Bradley Christensen - 2022-2024
#pragma once
#include <vector>
#include <string>



extern std::string UNNAMED_GUEST;



//----------------------------------------------------------------------------------------------------------------------
class Guest
{
public:

	bool IsValid() const;

	std::string ToString() const;
	void ImportFromString(std::string const& string);

	std::string m_name = UNNAMED_GUEST;
	std::vector<std::string> m_knownOtherGuests;
};

