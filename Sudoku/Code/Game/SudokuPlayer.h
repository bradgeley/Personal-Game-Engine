// Bradley Christensen - 2022-2023
#pragma once



class VertexBuffer;
class Camera;
class SudokuGrid;



class SudokuPlayer
{
public:

	SudokuPlayer();
	~SudokuPlayer();

	void Update(float deltaSeconds);
	void Render() const;

	void BeginGame(SudokuGrid* grid);

	void UpdateSelectedCell(float deltaSeconds);
	void UpdateNumberEntry();

public:

	Camera* m_camera;

protected:

	SudokuGrid* m_grid = nullptr;
	int m_selectedCellIndex = 0;
};

