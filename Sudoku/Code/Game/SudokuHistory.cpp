// Bradley Christensen - 2022-2023
#include "SudokuHistory.h"

#include "SudokuHistoryEvent.h"
#include "Engine/Core/EngineCommon.h"



//----------------------------------------------------------------------------------------------------------------------
void SudokuHistory::AddEvent(SudokuEvent const* event)
{
    if (!m_history.empty() && m_currentEventIndex != (int) m_history.size() - 1)
    {
        DeleteHistoryAfter(m_currentEventIndex);
    }
    
    // Then add the new event
    m_history.push_back(event);
    m_currentEventIndex++;
}



//----------------------------------------------------------------------------------------------------------------------
void SudokuHistory::Undo(SudokuGrid* grid, int numStepsToRevert)
{
	UNUSED(numStepsToRevert)
    
    if (m_currentEventIndex == -1)
    {
        // already at the minimum history
        return;
    }

    SudokuEvent const* event = m_history[m_currentEventIndex];
    event->UndoSelfOn(grid);
    m_currentEventIndex--;
}



//----------------------------------------------------------------------------------------------------------------------
void SudokuHistory::Redo(SudokuGrid* grid, int numStepsToRestore)
{
	UNUSED(numStepsToRestore)
    
    if (m_currentEventIndex == (int) m_history.size() - 1)
    {
        // already at the end
        return;
    }
    
    m_currentEventIndex++;
    SudokuEvent const* event = m_history[m_currentEventIndex];
    event->RestoreSelfOn(grid);
}



//----------------------------------------------------------------------------------------------------------------------
void SudokuHistory::DeleteHistoryAfter(int index)
{
    for (int i = index + 1; i < (int) m_history.size(); ++i)
    {
        delete m_history[i];
        m_history[i] = nullptr;
    }
    m_history.resize(index + 1);
}


