// Bradley Christensen - 2022-2023
#pragma once
#include "SudokuRuleSet.h"
#include "Engine/Math/Grid.h"



class VertexBuffer;



//----------------------------------------------------------------------------------------------------------------------
struct SudokuGridConfig
{
	IntVec2 m_dims;
	SudokuRuleSet m_rules;
	Grid<int> m_solution;
	Grid<int> m_startingState;
};



//----------------------------------------------------------------------------------------------------------------------
class SudokuGrid : public Grid<int>
{
public:

	SudokuGrid(SudokuGridConfig config);
	~SudokuGrid();

	void Update(float deltaSeconds);
	void Render() const;

	void RenderSelectedCells(std::vector<int> const& m_selectedCellIndices) const;

protected:

	void UpdateTextVerts();

protected:

	SudokuGridConfig m_config;

	VertexBuffer* m_staticGridVerts;
	VertexBuffer* m_selectedCellVerts;
	VertexBuffer* m_textVerts;
};

