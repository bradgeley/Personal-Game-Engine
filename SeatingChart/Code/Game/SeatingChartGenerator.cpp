// Bradley Christensen - 2022-2024
#include "SeatingChartGenerator.h"
#include "Engine/Core/ErrorUtils.h"
#include "Engine/Debug/DevConsole.h"
#include "SeatingChart.h"
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
	int numGuests = (int) guestList->m_guests.size();

	// Set the tables
	int maxTables = chartDef.m_maxNumTables;
	int maxGuestsPerTable = chartDef.m_maxGuestsPerTable;
	int maxSeats = maxTables * maxGuestsPerTable;

	// Figure out which guests are already seated
	std::vector<bool> isGuestAlreadySeated;
	isGuestAlreadySeated.resize(numGuests);

	for (int guestIndex = 0; guestIndex < numGuests; ++guestIndex)
	{
		std::string const& guestName = guestList->m_guests[guestIndex]->m_name;
		ASSERT_OR_DIE(!guestName.empty(), "Empty guest name");
		bool isSeated = (chart->GetTableForGuest(guestName) != -1);
		isGuestAlreadySeated[guestIndex] = isSeated;
	}

	// Generate wavefunctions for every seat, excluding ones that have already been seated
	for (int seatIndex = 0; seatIndex < maxSeats; ++seatIndex)
	{
		Seat seat;
		seat.m_seatIndex = seatIndex;
		for (int guestIndex = 0; guestIndex < numGuests; ++guestIndex)
		{
			std::string const& guest = guestList->m_guests[guestIndex]->m_name;
			if (!isGuestAlreadySeated[guestIndex])
			{
				seat.m_possibleGuests.push_back(guest);
			}
		}
		m_seats.emplace_back(seat);
	}

	int numGuestsSeated = 0;
	while (numGuestsSeated < numGuests)
	{
		// Collapse a single seat
		Seat* seat = GetLowestEntropySeat();
		if (seat)
		{
			CollapseSeat(seat);
			numGuestsSeated++;
		}
		else break;
	}

	// Combine smallest tables into tables that can fit them
	for (int i = 0; i < maxTables; ++i)
	{
		int numGuestsAtTable_i = chart->GetGuestsAtTable(i).size();
		if (numGuestsAtTable_i == 0)
		{
			continue;
		}
		int bestFitTableIndex = -1;
		int bestFitDifference = 999;
		for (int j = i + 1; j < maxTables; ++j)
		{
			int numGuestsAtTable_j = chart->GetGuestsAtTable(j).size();
			if (numGuestsAtTable_j == 0)
			{
				continue;
			}
			int combinedGuests = numGuestsAtTable_i + numGuestsAtTable_j;
			int combinedDifference = maxGuestsPerTable - combinedGuests;
			if (combinedDifference >= 0 && combinedDifference < bestFitDifference)
			{
				bestFitDifference = combinedDifference;
				bestFitTableIndex = j;
			}
		}
		if (bestFitTableIndex != -1)
		{
			chart->CombineTables(i, bestFitTableIndex);
		}
	}

	// Sort tables from most full to least full

	std::vector<std::string> sortedTables;
	sortedTables.resize(maxGuestsPerTable * maxTables);

	// Set threshold to max, if any tables have that number, put them next in the list, then lower threshold and repeat
	// Preserves table order.
	int numGuestsThreshold = maxGuestsPerTable;
	int currentSortedTableIndex = 0;
	while (numGuestsThreshold > 0)
	{
		for (int i = 0; i < maxTables; ++i)
		{
			int numGuestsAtTable = chart->GetGuestsAtTable(i).size();
			if (numGuestsAtTable == numGuestsThreshold)
			{
				for (int j = i * maxGuestsPerTable; j < (i + 1) * maxGuestsPerTable; ++j)
				{
					int indexInSortedList = (j % maxGuestsPerTable) + (maxGuestsPerTable * currentSortedTableIndex);
					sortedTables[indexInSortedList] = chart->m_seats[j];
				}
				++currentSortedTableIndex;
			}
		}
		--numGuestsThreshold;
	}

	chart->m_seats = sortedTables;

	// Error check
	chart->VerifyAllGuestsAreSeated();

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

	if (lowestIndex == -1)
	{
		// Failed
		return nullptr;
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
	int numGuestsPerTable = chart->m_def.m_maxGuestsPerTable;
	GuestList* guestList = chart->m_def.m_guestList;

	// Place the guest on the chart
	std::string placedGuest = seat->m_possibleGuests[randomIndex];
	bool placed = chart->PlaceGuest(seat->m_seatIndex, placedGuest);
	if (!placed)
	{
		return false;
	}

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
	int tableStartIndex = seat->m_seatIndex - seat->m_seatIndex % numGuestsPerTable;
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
