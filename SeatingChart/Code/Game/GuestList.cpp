// Bradley Christensen - 2022-2024
#include "GuestList.h"
#include "Guest.h"
#include "Engine/Core/FileUtils.h"
#include "Engine/Core/StringUtils.h"
#include "Engine/Core/ErrorUtils.h"



//----------------------------------------------------------------------------------------------------------------------
GuestList::GuestList(std::string const& filepath)
{
    LoadFromFile(filepath);
}



//----------------------------------------------------------------------------------------------------------------------
bool GuestList::LoadFromFile(std::string const& filepath)
{
    std::string guestList;
    int result = FileReadToString(filepath, guestList);
    ASSERT_OR_DIE(result > 0, "Guest list file is empty");

    Strings guests = SplitStringOnDelimeter(guestList, '\n');
    for (auto& guestString : guests)
    {
        Guest* guest = new Guest();
        guest->ImportFromString(guestString);
        m_guests.push_back(guest);
    }

    return true;
}



//----------------------------------------------------------------------------------------------------------------------
bool GuestList::SaveToFile(std::string const& filepath)
{
    std::string guestList;
    for (int i = 0; i < m_guests.size(); ++i)
    {
        auto& guest = m_guests[i];
        guestList.append(guest->ToString());

        if (i < m_guests.size() - 1)
        {
            guestList.push_back('\n');
        }
    }
    int result = FileWriteFromString(filepath, guestList);
    return result > 0;
}



//----------------------------------------------------------------------------------------------------------------------
Guest* GuestList::FindGuest(std::string const& guestName) const
{
    for (Guest* guest : m_guests)
    {
        if (strcmp(guestName.c_str(), guest->m_name.c_str()) == 0)
        {
            return guest;
        }
    }
    return nullptr;
}



//----------------------------------------------------------------------------------------------------------------------
bool GuestList::CanSitAtTableTogether(std::string const& guest, std::string const& otherGuest) const
{
    // True if either person has the other person on their known list.

    Guest* guestPtr = FindGuest(guest);
    for (std::string const& knownGuest : guestPtr->m_knownOtherGuests)
    {
        if (strcmp(knownGuest.c_str(), otherGuest.c_str()) == 0)
        {
            return true;
        }
    }

    Guest* otherGuestPtr = FindGuest(otherGuest);
    for (std::string const& otherKnownGuest : otherGuestPtr->m_knownOtherGuests)
    {
        if (strcmp(otherKnownGuest.c_str(), guest.c_str()) == 0)
        {
            return true;
        }
    }
    return false;
}
