// Bradley Christensen - 2022-2024
#include "Engine/Core/FileUtils.h"
#include "Engine/Core/StringUtils.h"
#include "SeatingChart.h"
#include "Table.h"



//----------------------------------------------------------------------------------------------------------------------
SeatingChart::SeatingChart(SeatingChartDefinition const& def) : m_def(def)
{
}



//----------------------------------------------------------------------------------------------------------------------
bool SeatingChart::PlaceGuest(int tableIndex, int seatIndex, std::string const& guestName)
{
	if (m_tables[tableIndex]->m_guests[seatIndex].size() > 0)
	{
		// Already a guest placed here
		return false;
	}
	m_tables[tableIndex]->m_guests[seatIndex] = guestName;
	return true;
}



//----------------------------------------------------------------------------------------------------------------------
std::string SeatingChart::ToString() const
{
	std::string result;
	for (int tableIndex = 0; tableIndex < m_tables.size(); ++tableIndex)
	{
		auto& table = m_tables[tableIndex];
		result.append(StringF("Table %i:\n", tableIndex));
		for (auto& guest : table->m_guests)
		{
			result.append(StringF("%s\r", guest.c_str()));
		}
		result.push_back('\r');
	}
	return result;
}



//----------------------------------------------------------------------------------------------------------------------
void SeatingChart::WriteToFile(std::string const& filepath) const
{
	std::string asString = ToString();
	FileWriteFromString(filepath, asString);
}



//----------------------------------------------------------------------------------------------------------------------
void SeatingChart::ReadFromFile(std::string const& filepath)
{
	std::string fileString;
	FileReadToString(filepath, fileString);
}
