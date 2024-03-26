// Bradley Christensen - 2022-2024
#include "Engine/Core/ErrorUtils.h"
#include "Engine/Core/FileUtils.h"
#include "Engine/Core/StringUtils.h"
#include "Engine/Debug/DevConsole.h"
#include "SeatingChart.h"
#include "GuestList.h"



//----------------------------------------------------------------------------------------------------------------------
SeatingChart::SeatingChart(SeatingChartDefinition const& def) : m_def(def)
{
	m_seats.resize(def.m_maxNumTables * def.m_maxGuestsPerTable);
}



//----------------------------------------------------------------------------------------------------------------------
bool SeatingChart::PlaceGuest(int seatIndex, std::string const& guestName)
{
	if (m_seats[seatIndex] != "")
	{
		// Already a guest placed here
		return false;
	}
	m_seats[seatIndex] = guestName;
	return true;
}



//----------------------------------------------------------------------------------------------------------------------
int SeatingChart::GetTableForGuest(std::string const& guest) const
{
	for (int i = 0; i < (int) m_seats.size(); ++i)
	{
		if (m_seats[i] == guest)
		{
			return i / m_def.m_maxGuestsPerTable;
		}
	}
	return -1;
}



//----------------------------------------------------------------------------------------------------------------------
Strings SeatingChart::GetGuestsAtTable(int tableId) const
{
	Strings result;
	int firstSeatAtTable = tableId * m_def.m_maxGuestsPerTable;
	int lastSeatAtTable = (tableId + 1) * m_def.m_maxGuestsPerTable;
	for (int i = firstSeatAtTable; i < lastSeatAtTable; ++i)
	{
		if (!m_seats[i].empty())
		{
			result.push_back(m_seats[i]);
		}
	}
	return result;
}



//----------------------------------------------------------------------------------------------------------------------
std::string SeatingChart::ToString() const
{
	int maxGuestsPerTable = m_def.m_maxGuestsPerTable;
	std::string result;
	for (int seatIndex = 0; seatIndex < m_seats.size(); ++seatIndex)
	{
		std::string const& seat = m_seats[seatIndex];
		if (seatIndex % maxGuestsPerTable == 0)
		{
			// New table starting
			result.append(StringF("\nTable %i:\n", seatIndex / maxGuestsPerTable));
		}
		if (seat.empty())
		{
			result.append(StringF("%s\n", seat.c_str()));
		}
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
	Strings lines = SplitStringOnAnyDelimeter(fileString, "\n\r");

	int maxGuestsAtTable = m_def.m_maxGuestsPerTable;

	// Remove empty lines
	for (int i = (int) lines.size() - 1; i >= 0; --i)
	{
		auto& line = lines[i];
		if (line == "")
		{
			lines.erase(lines.begin() + i);
		}
	}

	// Place guests at seats from manual seating chart
	int currentTable = -1;
	int currentSeatIndex = 0;
	bool isFinishedTable = false;
	for (std::string& line : lines)
	{
		if (line.find("Table") != std::string::npos)
		{
			isFinishedTable = line[0] == '*';
			currentTable++;
			currentSeatIndex = maxGuestsAtTable * currentTable; // reset seats to beginning of table
			continue;
		}

		std::string guest = line;
		TrimEdgeWhitespace(guest);
		ASSERT_OR_DIE(currentSeatIndex < m_seats.size(), StringF("Guest %s cannot fit into table %i", guest.c_str(), currentTable));

		m_seats[currentSeatIndex] = guest;
		currentSeatIndex++;
	}

	// Verify that those guests are in the guest list
	for (auto& seat : m_seats)
	{
		if (seat == "")
		{
			continue;
		}

		if (!m_def.m_guestList->FindGuest(seat))
		{
			g_devConsole->LogErrorF("Guest %s not found in guest list", seat.c_str());
		}
	}
}
