// Bradley Christensen - 2022-2023
#include "Engine/Renderer/Camera.h"
#include "Engine/Renderer/VertexBuffer.h"
#include "Engine/Renderer/VertexUtils.h"
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
}



void SudokuPlayer::Render() const
{
}



void SudokuPlayer::BeginGame(SudokuGrid* grid)
{
	float 
	m_camera->SetOrthoBounds();
}



void SudokuPlayer::UpdateSelectedCell(float deltaSeconds)
{
	UNUSED(deltaSeconds)

	// Update selection
	bool selectionChanged = false;
	if (g_input->WasMouseButtonJustPressed(0))
	{
		Vec2 clientRelativePos = g_input->GetMouseClientRelativePosition();
		Vec2 clientCellPos = clientRelativePos * Vec2(m_grid->GetWidth(), m_grid->GetHeight());
		IntVec2 cellCoords = IntVec2(clientCellPos); // round down
		int cellIndex = m_grid->GetIndexForCoords(cellCoords);
		if (cellIndex != m_selectedCellIndex)
		{
			m_selectedCellIndex = cellIndex;
		}
		else
		{
			m_selectedCellIndex = -1;
		}
		selectionChanged = true;
	}

	IntVec2 selectedCellCoords = m_grid->GetCoordsForIndex(m_selectedCellIndex);

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
			m_selectedCellIndex = m_grid->GetIndexForCoords(selectedCellCoords);
			selectionChanged = true;
			timeOfLastArrowMove = GetCurrentTimeSecondsF();
		}
		if (g_input->IsKeyDown(KeyCode::DOWN))
		{
			selectedCellCoords.y--;
			if (selectedCellCoords.y < 0.f)
			{
				selectedCellCoords.y = m_grid->GetHeight() - 1;
			}
			m_selectedCellIndex = m_grid->GetIndexForCoords(selectedCellCoords);
			selectionChanged = true;
			timeOfLastArrowMove = GetCurrentTimeSecondsF();
		}
		if (g_input->IsKeyDown(KeyCode::LEFT))
		{
			selectedCellCoords.x--;
			if (selectedCellCoords.x < 0.f)
			{
				selectedCellCoords.x = m_grid->GetWidth() - 1;
			}
			m_selectedCellIndex = m_grid->GetIndexForCoords(selectedCellCoords);
			selectionChanged = true;
			timeOfLastArrowMove = GetCurrentTimeSecondsF();
		}
		if (g_input->IsKeyDown(KeyCode::RIGHT))
		{
			selectedCellCoords.x++;
			if (selectedCellCoords.x >= m_grid->GetWidth())
			{
				selectedCellCoords.x = 0;
			}
			m_selectedCellIndex = m_grid->GetIndexForCoords(selectedCellCoords);
			selectionChanged = true;
			timeOfLastArrowMove = GetCurrentTimeSecondsF();
		}
	}


	if (selectionChanged)
	{
		// Update verts

	}
}

void SudokuPlayer::UpdateNumberEntry()
{
	if (m_selectedCellIndex == -1)
	{
		// no need to update bc selected cell doesn't exist
		return;
	}

	int numEntered = 0;
	for (int c = (int) '1'; c <= (int) '9'; ++c)
	{
		if (g_input->WasKeyJustPressed(c))
		{
			numEntered = c - (int) '1' + 1;
		}
	}
	if (numEntered != 0)
	{
		(*m_grid)[m_selectedCellIndex] = numEntered;
	}

	if (g_input->IsKeyDown(KeyCode::DEL) || g_input->IsKeyDown(KeyCode::BACKSPACE))
	{
		(*m_grid)[m_selectedCellIndex] = 0;
	}
}
