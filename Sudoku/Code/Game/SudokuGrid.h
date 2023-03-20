// Bradley Christensen - 2022-2023
#pragma once
#include "SudokuHistory.h"
#include "SudokuRuleSet.h"
#include "Engine/Math/Grid2D.h"
#include "Engine/Renderer/EngineConstantBuffers.h"
#include "Engine/Renderer/Rgba8.h"
#include "Game/GameCommon.h"



class Camera;
class VertexBuffer;



//----------------------------------------------------------------------------------------------------------------------
struct SudokuGridConfig
{
	// Rules/Initial Grid State
	IntVec2 m_dims = IntVec2(9, 9);
	SudokuRuleSet m_ruleSet;
	Grid2D<int> m_solution;
	Grid2D<int> m_startingState;
	Grid2D<Rgba8> m_cellShading;

	// Grid Lines
	float m_regionLineThickness				= 0.05f;
	float m_gridLineThickness				= 0.025f;

	// Color Palette
	std::vector<Rgba8> m_cellShadingPalette;
	float m_colorPaletteScale					= 0.5f;
	Rgba8 m_backgroundColor						= Rgba8::White;
	Rgba8 m_smallGridLineColor					= Rgba8::DarkGray;
	Rgba8 m_bigGridLineColor					= Rgba8::Black;
	Rgba8 m_givenDigitColor						= Rgba8::Black;
	Rgba8 m_enteredDigitColor					= Rgba8::Cerulean;
	Rgba8 m_wrongDigitColor						= Rgba8::Red;

	// Font Settings
	FontConstants m_bigDigitFontConstants		= FontConstants();
	FontConstants m_mediumDigitFontConstants	= FontConstants();
	FontConstants m_littleDigitFontConstants	= FontConstants();
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
	bool IsCellSelected(int index) const;
	bool IsCellSelected(IntVec2 const& coords) const;
	bool IsGivenDigit(int index) const;

	// Actions that apply to selected cells and save in the history log
	void MoveSelectedCell(EDirection direction);
	void ClearSelectedCells();
	void EnterCharacter(uint8_t character);
	void FillSelectedCells();

	// Do not save in the history log
	void SetCellFill(int index, Rgba8 const& tint);
	void SetCellCharacter(int index, uint8_t character);

	void IncrementColorPaletteIndex();
	void DecrementColorPaletteIndex();

	void UndoEvent();
	void RedoEvent();

protected:

	void UpdateTextVerts();
	void UpdateSelectedCellVerts() const;
	void AddVertsForSelectedCell(int index) const;
	void RenderColorPalette(Vec2 const& origin) const;

protected:

	SudokuGridConfig m_config;

	SudokuHistory m_history;

	// Grid state
	int m_currentColorPaletteIndex = 0;
	Grid2D<bool> m_selectedCells;
	Grid2D<Rgba8> m_cellShading;

	VertexBuffer* m_cellShadingVerts = nullptr;
	VertexBuffer* m_staticGridVerts = nullptr;
	VertexBuffer* m_selectedCellVerts = nullptr;
	VertexBuffer* m_textVerts = nullptr;
};





