// Bradley Christensen - 2022-2023
#pragma once
#include "SudokuRuleSet.h"
#include "Engine/DataStructures/NamedProperties.h"
#include "Engine/Math/Grid.h"



class VertexBuffer;



//----------------------------------------------------------------------------------------------------------------------
struct SudokuGridConfig
{
	IntVec2 m_dims = IntVec2(9, 9);
	SudokuRuleSet m_ruleSet;
	Grid<int> m_solution;
	Grid<int> m_startingState;
};



//----------------------------------------------------------------------------------------------------------------------
class SudokuGrid : public Grid<int>
{
public:

	SudokuGrid(SudokuGridConfig const& config);
	~SudokuGrid();

	void Startup();
	void Update(float deltaSeconds);
	void Render() const;
	void Shutdown();

	void RenderSelectedCells(std::vector<int> const& m_selectedCellIndices) const;

protected:

	void UpdateTextVerts();

protected:

	SudokuGridConfig m_config;

	VertexBuffer* m_staticGridVerts;
	VertexBuffer* m_selectedCellVerts;
	VertexBuffer* m_textVerts;
};

