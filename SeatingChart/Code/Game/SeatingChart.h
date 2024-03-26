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
};



//----------------------------------------------------------------------------------------------------------------------
class SeatingChart
{
public:

	SeatingChart(SeatingChartDefinition const& def);

	bool PlaceGuest(int seatIndex, std::string const& guestName);
	int GetTableForGuest(std::string const& guest) const;
	Strings GetGuestsAtTable(int tableId) const;

	std::string ToString() const;
	void WriteToFile(std::string const& filepath) const;
	void ReadFromFile(std::string const& filepath);

	SeatingChartDefinition m_def;
	
	// Tables 1 is index 0-10, etc
	std::vector<std::string> m_seats;
};

