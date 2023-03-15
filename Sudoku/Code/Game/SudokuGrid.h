// Bradley Christensen - 2022-2023
#pragma once
#include "SudokuRuleSet.h"
#include "Engine/Math/Grid2D.h"
#include "Engine/Renderer/Rgba8.h"
#include "Game/GameCommon.h"



class VertexBuffer;



//----------------------------------------------------------------------------------------------------------------------
struct SudokuGridConfig
{
	IntVec2 m_dims = IntVec2(9, 9);
	SudokuRuleSet m_ruleSet;
	Grid2D<int> m_solution;
	Grid2D<int> m_startingState;
};



//----------------------------------------------------------------------------------------------------------------------
class SudokuGrid : public Grid2D<int>
{
public:

	SudokuGrid(SudokuGridConfig const& config);
	~SudokuGrid();

	void Startup();
	void Update(float deltaSeconds);
	void Render() const;
	void Shutdown();

	void SelectCell(int index);
	void SelectCell(IntVec2 const& coords);
	void DeselectCell(int index);
	void DeselectCell(IntVec2 const& coords);
	void DeselectAllCells();

	int CountSelectedCells() const;
	int GetSelectedCellIndex() const;
	void MoveSelectedCell(EDirection direction);

	// Actions that apply to selected cells
	void ClearCell();
	void EnterCharacter(uint8_t character);

protected:

	void UpdateTextVerts();
	void UpdateSelectedCellVerts() const;
	void AddVertsForSelectedCell(int index) const;

protected:

	SudokuGridConfig m_config;

	Grid2D<bool> m_selectedCells;
	Grid2D<Rgba8> m_cellFill;

	VertexBuffer* m_staticGridVerts = nullptr;
	VertexBuffer* m_selectedCellVerts = nullptr;
	VertexBuffer* m_textVerts = nullptr;
};

