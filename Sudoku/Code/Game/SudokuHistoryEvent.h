// Bradley Christensen - 2022-2023
#pragma once
#include <vector>

#include "Engine/Renderer/Rgba8.h"



class SudokuGrid;



//----------------------------------------------------------------------------------------------------------------------
struct SudokuEvent
{
    virtual ~SudokuEvent() = default;
    virtual void UndoSelfOn(SudokuGrid* grid) const = 0;
    virtual void RestoreSelfOn(SudokuGrid* grid) const = 0;
};



//----------------------------------------------------------------------------------------------------------------------
// This event occurs when the player enters digits in any way
//
struct SudokuEventChangeDigits : public SudokuEvent
{
    virtual void UndoSelfOn(SudokuGrid* grid) const override;
    virtual void RestoreSelfOn(SudokuGrid* grid) const override;
    
    std::vector<uint8_t> m_indices;         // indices that were changed
    std::vector<uint8_t> m_digitsBefore;    // digits they were before
    std::vector<uint8_t> m_digitsAfter;     // digits they were changed to
};



//----------------------------------------------------------------------------------------------------------------------
// This event occurs when the player enters digits in any way
//
struct SudokuEventFillColor : public SudokuEvent
{
    virtual void UndoSelfOn(SudokuGrid* grid) const override;
    virtual void RestoreSelfOn(SudokuGrid* grid) const override;
    
    std::vector<uint8_t> m_indices;     // indices that were changed
    std::vector<Rgba8> m_tintBefore;    // tint they were before
    std::vector<Rgba8> m_tintAfter;     // tint they were changed to
};