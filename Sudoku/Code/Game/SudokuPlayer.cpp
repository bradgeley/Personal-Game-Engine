// Bradley Christensen - 2022-2023
#include "Engine/Renderer/Camera.h"
#include "Engine/Input/InputSystem.h"
#include "Engine/Core/Time.h"
#include "Engine/Math/Grid.h"
#include "Engine/Core/EngineCommon.h"
#include "SudokuPlayer.h"
#include "SudokuGrid.h"
#include "Engine/Renderer/Window.h"



class VertexBuffer;



SudokuPlayer::SudokuPlayer()
{
	m_camera = new Camera();
}



SudokuPlayer::~SudokuPlayer()
{
	if (m_camera)
	{
		delete m_camera;
	}
}



void SudokuPlayer::Startup()
{
	g_window->m_charInputEvent.SubscribeMethod(this, &SudokuPlayer::OnCharDown);
	g_window->m_keyDownEvent.SubscribeMethod(this, &SudokuPlayer::OnKeyDown);
}



void SudokuPlayer::Update(float deltaSeconds)
{
	UpdateSelectedCell(deltaSeconds);
	UpdateArrowKeysSelectedCellMovement(deltaSeconds);
}



void SudokuPlayer::Render() const
{
	m_grid->RenderSelectedCells(m_selectedCellIndices);
}



void SudokuPlayer::Shutdown()
{
	g_window->m_charInputEvent.UnsubscribeMethod(this, &SudokuPlayer::OnCharDown);
	g_window->m_keyDownEvent.UnsubscribeMethod(this, &SudokuPlayer::OnKeyDown);
}



void SudokuPlayer::BeginGame(SudokuGrid* grid)
{
	m_grid = grid;
	Vec3 boardMins = Vec3::ZeroVector;
	Vec3 boardMaxs = Vec3(grid->GetDimensions(), 1.f);
	m_camera->SetOrthoBounds(boardMins, boardMaxs);
}



void SudokuPlayer::DeselectAllCells()
{
	m_selectedCellIndices.clear();
}



void SudokuPlayer::UpdateSelectedCell(float deltaSeconds)
{
	UNUSED(deltaSeconds)
	
	// Right mouse button, clear cells
	if (g_input->WasMouseButtonJustPressed(1))
	{
		DeselectAllCells();
	}
	
	bool isLeftMouseButtonDown = g_input->IsMouseButtonDown(0);
	if (!isLeftMouseButtonDown)
	{
		return;
	}
	
	// Get cell at the current mouse position
	Vec2 clientRelativePos = g_input->GetMouseClientRelativePosition();
	Vec2 clientCellPos = clientRelativePos * Vec2(m_grid->GetWidth(), m_grid->GetHeight());
	IntVec2 cellCoords = IntVec2(clientCellPos); // round down
	int cellIndex = m_grid->GetIndexForCoords(cellCoords);
	
	// Update selection
	if (g_input->WasMouseButtonJustPressed(0))
	{
		if (!g_input->IsKeyDown(KeyCode::SHIFT))
		{
			DeselectAllCells();
		}
		SelectCell(cellIndex);
	}

	// If dragging, select all cells
	if (isLeftMouseButtonDown)
	{ 
		SelectCell(cellIndex);
	}
}



void SudokuPlayer::UpdateArrowKeysSelectedCellMovement(float deltaSeconds)
{
	UNUSED(deltaSeconds)
	if (m_selectedCellIndices.size() != 1)
	{
		// Only do this if there's 1 selected cell
		return;
	}
	
	auto& selectedCellIndex = m_selectedCellIndices[0];
	IntVec2 selectedCellCoords = m_grid->GetCoordsForIndex(selectedCellIndex);
	constexpr float minSecondsBetweenArrowMoves = 0.1f;
	static float timeOfLastArrowMove = 0.f;
	if (GetCurrentTimeSecondsF() - timeOfLastArrowMove > minSecondsBetweenArrowMoves)
	{
		if (g_input->IsKeyDown(KeyCode::UP))
		{
			selectedCellCoords.y++;
			if (selectedCellCoords.y >= m_grid->GetHeight())
			{
				selectedCellCoords.y = 0;
			}
			timeOfLastArrowMove = GetCurrentTimeSecondsF();
		}
		if (g_input->IsKeyDown(KeyCode::DOWN))
		{
			selectedCellCoords.y--;
			if (selectedCellCoords.y < 0)
			{
				selectedCellCoords.y = m_grid->GetHeight() - 1;
			}
			timeOfLastArrowMove = GetCurrentTimeSecondsF();
		}
		if (g_input->IsKeyDown(KeyCode::LEFT))
		{
			selectedCellCoords.x--;
			if (selectedCellCoords.x < 0)
			{
				selectedCellCoords.x = m_grid->GetWidth() - 1;
			}
			timeOfLastArrowMove = GetCurrentTimeSecondsF();
		}
		if (g_input->IsKeyDown(KeyCode::RIGHT))
		{
			selectedCellCoords.x++;
			if (selectedCellCoords.x >= m_grid->GetWidth())
			{
				selectedCellCoords.x = 0;
			}
			timeOfLastArrowMove = GetCurrentTimeSecondsF();
		}
		selectedCellIndex = m_grid->GetIndexForCoords(selectedCellCoords);
	}
}



bool SudokuPlayer::OnCharDown(NamedProperties& args)
{
	int c = args.Get("Char", -1);
	for (auto& selectedCell : m_selectedCellIndices)
	{
		if (!m_grid->IsValidIndex(selectedCell))
		{
			continue;
		}
		
		(*m_grid)[selectedCell] = c;

		if (g_input->IsKeyDown(KeyCode::DEL) || g_input->IsKeyDown(KeyCode::BACKSPACE))
		{
			(*m_grid)[selectedCell] = 0;
		}
	}
	return true;
}



bool SudokuPlayer::OnKeyDown(NamedProperties& args)
{
	int key = args.Get("Key", -1);
	UNUSED(key)
	return true;
}



void SudokuPlayer::SelectCell(int index)
{
	for (auto& cell : m_selectedCellIndices)
	{
		if (cell == index)
		{
			return;
		}
	}
	m_selectedCellIndices.emplace_back(index);
}
