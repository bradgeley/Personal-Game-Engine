// Bradley Christensen - 2022-2023
#include "Engine/Renderer/Camera.h"
#include "Engine/Input/InputSystem.h"
#include "Engine/Core/Time.h"
#include "Engine/Math/Grid.h"
#include "Engine/Core/EngineCommon.h"
#include "SudokuPlayer.h"
#include "SudokuGrid.h"



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



void SudokuPlayer::Update(float deltaSeconds)
{
	UpdateSelectedCell(deltaSeconds);
	UpdateNumberEntry();
	if (m_selectedCellIndices.size() == 1)
	{
		UpdateArrowKeysSelectedCellMovement(deltaSeconds);
	}
}



void SudokuPlayer::Render() const
{
	m_grid->RenderSelectedCells(m_selectedCellIndices);
}



void SudokuPlayer::BeginGame(SudokuGrid* grid)
{
	m_grid = grid;
	Vec3 boardMins = Vec3::ZeroVector;
	Vec3 boardMaxs = Vec3(grid->GetDimensions(), 1.f);
	m_camera->SetOrthoBounds(boardMins, boardMaxs);
}



void SudokuPlayer::UpdateSelectedCell(float deltaSeconds)
{
	UNUSED(deltaSeconds)

	// Update selection
	if (g_input->WasMouseButtonJustPressed(0))
	{
		Vec2 clientRelativePos = g_input->GetMouseClientRelativePosition();
		Vec2 clientCellPos = clientRelativePos * Vec2(m_grid->GetWidth(), m_grid->GetHeight());
		IntVec2 cellCoords = IntVec2(clientCellPos); // round down
		int cellIndex = m_grid->GetIndexForCoords(cellCoords);
		SelectCell(cellIndex);
	}
}



void SudokuPlayer::UpdateNumberEntry()
{
	for (auto& selectedCell : m_selectedCellIndices)
	{
		if (!m_grid->IsValidIndex(selectedCell))
		{
			return;
		}

		int numEntered = 0;
		for (int c = (int) '0'; c <= (int) '9'; ++c)
		{
			if (g_input->WasKeyJustPressed(c))
			{
				(*m_grid)[selectedCell] = c - (int) '0';
			}
		}

		if (g_input->IsKeyDown(KeyCode::DEL) || g_input->IsKeyDown(KeyCode::BACKSPACE))
		{
			(*m_grid)[selectedCell] = 0;
		}
	}
}



void SudokuPlayer::UpdateArrowKeysSelectedCellMovement(float deltaSeconds)
{
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
