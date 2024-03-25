// Bradley Christensen - 2022-2024
#pragma once
#include <vector>
#include <string>



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

	bool PlaceGuest(int tableIndex, int seatIndex, std::string const& guestName);

	std::string ToString() const;
	void WriteToFile(std::string const& filepath) const;
	void ReadFromFile(std::string const& filepath);

	SeatingChartDefinition m_def;
	std::vector<Table*> m_tables;
};

