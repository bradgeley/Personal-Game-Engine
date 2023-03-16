// Bradley Christensen - 2022-2023
#pragma once
#include "Engine/Math/Vec2.h"



class VertexBuffer;
class Camera;
class SudokuGrid;



class SudokuPlayer
{
public:

	SudokuPlayer();
	~SudokuPlayer();

	void Startup();
	void Update(float deltaSeconds);
	void Render() const;
	void EndFrame();
	void Shutdown();

	void BeginGame(SudokuGrid* grid);

private:
	
	void UpdateSelectedCell(float deltaSeconds);
	void UpdateArrowKeysSelectedCellMovement(float deltaSeconds);
	void SelectCellsInLine(Vec2 const& start, Vec2 const& end) const;
	
	bool OnCharDown(NamedProperties& args);
	bool OnKeyDown(NamedProperties& args);
	
public:

	Camera* m_camera;

protected:

	SudokuGrid* m_grid = nullptr;
	Vec2 m_mouseClientRelativePosLastFrame = Vec2(0.5f, 0.5f);
};

