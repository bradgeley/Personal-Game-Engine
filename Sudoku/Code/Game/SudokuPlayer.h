// Bradley Christensen - 2022-2023
#pragma once
#include "Engine/Math/Vec2.h"
#include "Engine/Renderer/Rgba8.h"



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
	void UpdateFill();
	
	void SelectCellsInLine(Vec2 const& start, Vec2 const& end) const;
	void RenderColorPalette() const;
	
	bool OnCharDown(NamedProperties& args);
	bool OnKeyDown(NamedProperties& args);
	bool OnMouseButtonDown(NamedProperties& args);
	bool OnMouseWheelUp(NamedProperties& args);
	bool OnMouseWheelDown(NamedProperties& args);
	
public:

	Camera* m_camera;

protected:

	SudokuGrid* m_grid = nullptr;

	// Color Palette
	int m_currentColorPaletteIndex = 0;
	std::vector<Rgba8> m_colorPalette;
	
	Vec2 m_mouseClientRelativePosLastFrame = Vec2(0.5f, 0.5f);
};

