// Bradley Christensen - 2022-2024
#include "GuestList.h"
#include "Guest.h"
#include "SeatingChart.h"
#include "Engine/Core/FileUtils.h"
#include "Engine/Core/StringUtils.h"
#include "Engine/Core/ErrorUtils.h"
#include "Engine/Debug/DevConsole.h"
#include <algorithm>



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

    CheckForDuplicates();

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



//----------------------------------------------------------------------------------------------------------------------
void GuestList::CheckForDuplicates()
{
    int numGuests = (int) m_guests.size();
    for (int i = 0; i < numGuests; ++i)
    {
        auto& guestA = m_guests[i];
        for (int j = i + 1; j < numGuests; ++j)
        {
            auto& guestB = m_guests[j];
            if (GetToLower(guestA->m_name) == GetToLower(guestB->m_name))
            {
                g_devConsole->LogErrorF("Duplicate guest: %s at indices %i and %i", guestA->m_name.c_str(), i, j);
            }
        }
    }
}



//----------------------------------------------------------------------------------------------------------------------
bool CompareGuests(Guest* a, const Guest* b)
{
    return (a->m_name < b->m_name);
}



//----------------------------------------------------------------------------------------------------------------------
void GuestList::Alphebetize()
{
    std::sort(m_guests.begin(), m_guests.end(), CompareGuests);
}



//----------------------------------------------------------------------------------------------------------------------
void GuestList::PopulateFriendsFromSeatingChart(SeatingChart* chart)
{
    for (auto& guest : m_guests)
    {
        int tableId = chart->GetTableForGuest(guest->m_name);
        if (tableId == -1)
        {
            // Not seated in the seating chart
            continue;
        }

        Strings friends = chart->GetGuestsAtTable(tableId);
        for (auto& person : friends)
        {
            if (!person.empty() && !(person == guest->m_name))
            {
                guest->AddFriend(person);
            }
        }
    }
}
