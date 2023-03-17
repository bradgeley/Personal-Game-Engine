// Bradley Christensen - 2022-2023
#include "SudokuHistoryEvent.h"
#include "SudokuGrid.h"



//----------------------------------------------------------------------------------------------------------------------
void SudokuEventChangeDigits::UndoSelfOn(SudokuGrid* grid) const
{
    for (int i = 0; i < (int) m_indices.size(); ++i)
    {
        uint8_t const& index = m_indices[i];
        uint8_t const& digit = m_digitsBefore[i];
        grid->SetCellCharacter(index, digit);
    }
}



//----------------------------------------------------------------------------------------------------------------------
void SudokuEventChangeDigits::RestoreSelfOn(SudokuGrid* grid) const
{
    for (int i = 0; i < (int) m_indices.size(); ++i)
    {
        uint8_t const& index = m_indices[i];
        uint8_t const& digit = m_digitsAfter[i];
        grid->SetCellCharacter(index, digit);
    }
}



//----------------------------------------------------------------------------------------------------------------------
void SudokuEventFillColor::UndoSelfOn(SudokuGrid* grid) const
{
    for (int i = 0; i < (int) m_indices.size(); ++i)
    {
        uint8_t const& index = m_indices[i];
        Rgba8 const& tint = m_tintBefore[i];
        grid->SetCellFill(index, tint);
    }
}



//----------------------------------------------------------------------------------------------------------------------
void SudokuEventFillColor::RestoreSelfOn(SudokuGrid* grid) const
{
    for (int i = 0; i < (int) m_indices.size(); ++i)
    {
        uint8_t const& index = m_indices[i];
        Rgba8 const& tint = m_tintAfter[i];
        grid->SetCellFill(index, tint);
    }
}
