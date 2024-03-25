// Bradley Christensen - 2022-2024
#pragma once
#include <vector>
#include <string>



class Guest;



//----------------------------------------------------------------------------------------------------------------------
class GuestList
{
public:

	GuestList(std::string const& filepath);

	bool LoadFromFile(std::string const& filepath);
	bool SaveToFile(std::string const& filepath);

	Guest* FindGuest(std::string const& guestName) const;
	bool CanSitAtTableTogether(std::string const& guest, std::string const& otherGuest) const;

	std::vector<Guest*> m_guests;
};

