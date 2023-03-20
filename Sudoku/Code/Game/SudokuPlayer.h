// Bradley Christensen - 2022-2023
#pragma once
#include "Engine/Math/Vec2.h"



class VertexBuffer;
class Camera;
class SudokuGrid;



//----------------------------------------------------------------------------------------------------------------------
class SudokuPlayer
{
public:

	SudokuPlayer();
	~SudokuPlayer();

	void Startup();
	void Update(float deltaSeconds);
	void BeginCamera() const;
	void EndFrame();
	void Shutdown();

	void BeginGame(SudokuGrid* grid);

private:
	
	void UpdateSelectedCell(float deltaSeconds);
	void UpdateArrowKeysSelectedCellMovement(float deltaSeconds);
	void UpdateFill();
	
	void SetCellsSelectedInLine(Vec2 const& start, Vec2 const& end, bool isSelected) const;
	void GetColorPalette() const;
	
	bool OnCharDown(NamedProperties& args);
	bool OnKeyDown(NamedProperties& args);
	bool OnMouseWheelUp(NamedProperties& args);
	bool OnMouseWheelDown(NamedProperties& args);
	bool OnDoubleClick(NamedProperties& args);
	
public:

	Camera* m_camera;

protected:

	SudokuGrid* m_grid = nullptr;

	bool m_selectMode = true;
	Vec2 m_mouseClientRelativePosLastFrame = Vec2(0.5f, 0.5f);
};

