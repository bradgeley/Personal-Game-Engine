// Bradley Christensen - 2022-2024
#pragma once
#include <vector>
#include <string>



class SeatingChart;



//----------------------------------------------------------------------------------------------------------------------
// Data structure for generating wavefunctions for each seat and collapsing them
struct Seat
{
	int m_seatIndex = -1;
	std::vector<std::string> m_possibleGuests;
};



//----------------------------------------------------------------------------------------------------------------------
struct SeatingChartGeneratorDefinition
{
	SeatingChart* m_seatingChart = nullptr;
};



//----------------------------------------------------------------------------------------------------------------------
class SeatingChartGenerator
{
public:

	SeatingChartGenerator(SeatingChartGeneratorDefinition const& def);

	void Generate();

	Seat* GetLowestEntropySeat(bool getRandomIfTie = true);
	bool CollapseSeat(Seat* seat);

private:

	SeatingChartGeneratorDefinition m_def;
	std::vector<Seat> m_seats;
};

