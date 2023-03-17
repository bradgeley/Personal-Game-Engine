// Bradley Christensen - 2022-2023
#pragma once
#include <vector>



class SudokuGrid;
struct SudokuEvent;



//----------------------------------------------------------------------------------------------------------------------
class SudokuHistory
{
public:

    void AddEvent(SudokuEvent const* event);
    void Revert(SudokuGrid* grid, int numStepsToRevert = 1);
    void Restore(SudokuGrid* grid, int numStepsToRestore = 1);
    void DeleteHistoryAfter(int index);

protected:

    int m_currentEventIndex = -1;
    std::vector<SudokuEvent const*> m_history;
};