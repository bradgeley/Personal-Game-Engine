// Bradley Christensen - 2022-2024
#include "SeatingChartGenerator.h"
#include "Engine/Core/ErrorUtils.h"
#include "SeatingChart.h"
#include "Table.h"
#include "Guest.h"
#include "GuestList.h"
#include "Engine/Math/RandomNumberGenerator.h"



//----------------------------------------------------------------------------------------------------------------------
SeatingChartGenerator::SeatingChartGenerator(SeatingChartGeneratorDefinition const& def) : m_def(def)
{

}



//----------------------------------------------------------------------------------------------------------------------
void SeatingChartGenerator::Generate()
{
	SeatingChart* chart = m_def.m_seatingChart;
	ASSERT_OR_DIE(chart, "Tried to generate from null seating chart");

	SeatingChartDefinition const& chartDef = chart->m_def;
	GuestList const* guestList = chartDef.m_guestList;

	// Set the tables
	int maxTables = chartDef.m_maxNumTables;
	int maxSeats = chartDef.m_maxGuestsPerTable;
	for (int i = 0; i < maxTables; ++i)
	{
		Table* table = new Table();
		table->m_guests.resize(chartDef.m_maxGuestsPerTable);
		chart->m_tables.push_back(table);
	}

	// Generate wavefunctions for every seat
	for (int tableIndex = 0; tableIndex < maxTables; ++tableIndex)
	{
		for (int seatIndex = 0; seatIndex < maxSeats; ++seatIndex)
		{
			Seat seat;
			seat.m_tableIndex = tableIndex;
			seat.m_seatIndex = seatIndex;
			for (Guest* guest : guestList->m_guests)
			{
				seat.m_possibleGuests.push_back(guest->m_name);
			}
			m_seats.emplace_back(seat);
		}
	}

	// Collapse a single seat
	int numGuestsSeated = 0;
	int numGuests = guestList->m_guests.size();
	while (numGuestsSeated < numGuests)
	{
		Seat* seat = GetLowestEntropySeat();
		CollapseSeat(seat);
		numGuestsSeated++;
	}
	m_def.m_seatingChart->WriteToFile(chartDef.m_saveFilepath);
}



//----------------------------------------------------------------------------------------------------------------------
Seat* SeatingChartGenerator::GetLowestEntropySeat(bool getRandomIfTie)
{
	int lowestIndex = -1;
	int lowestEntropy = INT_MAX;
	for (int seatIndex = 0; seatIndex < m_seats.size(); ++seatIndex)
	{
		auto& seat = m_seats[seatIndex];
		int entropy = (int) seat.m_possibleGuests.size();
		if (entropy == 0)
		{
			// already filled
			continue;
		}
		if (entropy < lowestEntropy)
		{
			lowestIndex = seatIndex;
			lowestEntropy = entropy;
		}
	}

	Seat* result = &m_seats[lowestIndex];

	if (getRandomIfTie)
	{
		// Go back through the list and get a list of all the indices of seats who tie
		std::vector<int> indicesOfTiedSeats;
		for (int seatIndex = 0; seatIndex < m_seats.size(); ++seatIndex)
		{
			auto& seat = m_seats[seatIndex];
			int entropy = (int) seat.m_possibleGuests.size();
			if (entropy == lowestEntropy)
			{
				indicesOfTiedSeats.push_back(seatIndex);
			}
		}

		if (indicesOfTiedSeats.size() > 0)
		{
			// Get a random value in indicesOfTiedSeats, which is the index of the random seat
			int randomInt = g_rng->GetRandomIntInRange(0, (int) indicesOfTiedSeats.size() - 1);
			int indexOfRandomSeat = indicesOfTiedSeats[randomInt];

			// Get the random seat by using the random index
			result = &m_seats[indexOfRandomSeat];
		}
	}

	return result;
}



//----------------------------------------------------------------------------------------------------------------------
bool SeatingChartGenerator::CollapseSeat(Seat* seat)
{
	int numPossibleGuests = (int) seat->m_possibleGuests.size();
	if (numPossibleGuests == 0)
	{
		return false;
	}
	int randomIndex = g_rng->GetRandomIntInRange(0, numPossibleGuests - 1);

	SeatingChart* chart = m_def.m_seatingChart;
	int maxTableSize = chart->m_def.m_maxGuestsPerTable;
	//int numTables = chart->m_def.m_maxNumTables;
	GuestList* guestList = chart->m_def.m_guestList;

	std::string placedGuest = seat->m_possibleGuests[randomIndex];

	// Place the guest on the chart
	chart->PlaceGuest(seat->m_tableIndex, seat->m_seatIndex, placedGuest);

	// Clear out all possible guests for this seat, bc its already filled
	seat->m_possibleGuests.clear();

	// Take that guest off of every seat's possible guests
	for (auto& seatIt : m_seats)
	{
		for (int i = 0; i < seatIt.m_possibleGuests.size(); ++i)
		{
			auto& possibleGuest = seatIt.m_possibleGuests[i];
			if (strcmp(possibleGuest.c_str(), placedGuest.c_str()) == 0)
			{
				seatIt.m_possibleGuests.erase(seatIt.m_possibleGuests.begin() + i);
				break;
			}
		}
	}

	// At that table, clear out any guests who can't sit with the placed guest
	int tableStartIndex = seat->m_tableIndex * maxTableSize;
	int tableEndIndex = tableStartIndex + maxTableSize;
	for (int seatIndex = tableStartIndex; seatIndex < tableEndIndex; ++seatIndex)
	{
		Seat* seatToCheck = &m_seats[seatIndex];
		if (seatToCheck == seat)
		{
			continue;
		}

		for (int possibleGuestIndex = (int) seatToCheck->m_possibleGuests.size() - 1; possibleGuestIndex >= 0; --possibleGuestIndex)
		{
			auto& possibleGuest = seatToCheck->m_possibleGuests[possibleGuestIndex];
			if (!guestList->CanSitAtTableTogether(possibleGuest, placedGuest))
			{
				seatToCheck->m_possibleGuests.erase(seatToCheck->m_possibleGuests.begin() + possibleGuestIndex);
			}
		}
	}

	return true;
}
