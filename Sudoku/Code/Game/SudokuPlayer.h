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

	void Startup();
	void Update(float deltaSeconds);
	void Shutdown();

	void BeginGame(SudokuGrid* grid);

private:
	
	void UpdateSelectedCell(float deltaSeconds);
	void UpdateArrowKeysSelectedCellMovement(float deltaSeconds);
	
	bool OnCharDown(NamedProperties& args);
	bool OnKeyDown(NamedProperties& args);
	
public:

	Camera* m_camera;

protected:

	SudokuGrid* m_grid = nullptr;
};

