// Bradley Christensen - 2022-2024
#pragma once
#include "Engine/Core/StringUtils.h"
#include <vector>



class GuestList;
class Table;



//----------------------------------------------------------------------------------------------------------------------
struct SeatingChartDefinition
{
	GuestList* m_guestList = nullptr;
	int m_maxNumTables = 16;
	int m_maxGuestsPerTable = 10;
	std::string m_saveFilepath = "Data/SeatingChart.txt";
	std::string m_seatBlockedTag = "-Empty Seat-";
};



//----------------------------------------------------------------------------------------------------------------------
class SeatingChart
{
public:

	SeatingChart(SeatingChartDefinition const& def);

	bool PlaceGuest(int seatIndex, std::string const& guestName);
	bool RemoveGuest(int seatIndex);
	int GetTableForGuest(std::string const& guest) const;
	Strings GetGuestsAtTable(int tableId) const;
	void BlockRemainingSeatsAtTable(int tableId);
	bool CombineTables(int tableA, int tableB);
	bool VerifyAllGuestsAreSeated() const;

	std::string ToString() const;
	void WriteToFile(std::string const& filepath) const;
	void ReadFromFile(std::string const& filepath, bool onlyFinishedTables);

	SeatingChartDefinition m_def;
	
	// Tables 1 is index 0-10, etc
	std::vector<std::string> m_seats;
};

