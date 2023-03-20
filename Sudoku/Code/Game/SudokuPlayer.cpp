// Bradley Christensen - 2022-2023
#include "Engine/Renderer/Camera.h"
#include "Engine/Input/InputSystem.h"
#include "Engine/Core/Time.h"
#include "Engine/Math/Grid2D.h"
#include "Engine/Core/EngineCommon.h"
#include "Engine/DataStructures/NamedProperties.h"
#include "SudokuPlayer.h"
#include "SudokuGrid.h"
#include "Engine/Math/GeometryUtils.h"
#include "Engine/Math/MathUtils.h"
#include "Engine/Renderer/Renderer.h"



class VertexBuffer;



//----------------------------------------------------------------------------------------------------------------------
SudokuPlayer::SudokuPlayer()
{
	m_camera = new Camera();
}



//----------------------------------------------------------------------------------------------------------------------
SudokuPlayer::~SudokuPlayer()
{
	delete m_camera;
	m_camera =  nullptr;
}



//----------------------------------------------------------------------------------------------------------------------
void SudokuPlayer::Startup()
{
	g_input->m_charInputEvent.SubscribeMethod(this, &SudokuPlayer::OnCharDown);
	g_input->m_keyDownEvent.SubscribeMethod(this, &SudokuPlayer::OnKeyDown);
	g_input->m_mouseWheelUpEvent.SubscribeMethod(this, &SudokuPlayer::OnMouseWheelUp);
	g_input->m_mouseWheelDownEvent.SubscribeMethod(this, &SudokuPlayer::OnMouseWheelDown);
}



//----------------------------------------------------------------------------------------------------------------------
void SudokuPlayer::Update(float deltaSeconds)
{
	UpdateSelectedCell(deltaSeconds);
	UpdateArrowKeysSelectedCellMovement(deltaSeconds);
	UpdateFill();
}



//----------------------------------------------------------------------------------------------------------------------
void SudokuPlayer::BeginCamera() const
{
	g_renderer->BeginCamera(*m_camera);
}



//----------------------------------------------------------------------------------------------------------------------
void SudokuPlayer::EndFrame()
{
	m_mouseClientRelativePosLastFrame = g_input->GetMouseClientRelativePosition();
}



//----------------------------------------------------------------------------------------------------------------------
void SudokuPlayer::Shutdown()
{
	g_input->m_charInputEvent.UnsubscribeMethod(this, &SudokuPlayer::OnCharDown);
	g_input->m_keyDownEvent.UnsubscribeMethod(this, &SudokuPlayer::OnKeyDown);
}



//----------------------------------------------------------------------------------------------------------------------
void SudokuPlayer::BeginGame(SudokuGrid* grid)
{
	m_grid = grid;
	Vec3 boardMins = Vec3::ZeroVector;
	Vec3 boardMaxs = Vec3(grid->GetDimensions(), 1.f);
	m_camera->SetOrthoBounds(boardMins - Vec3(1.f, 1.f), boardMaxs + Vec3(1.f, 1.f));
}



//----------------------------------------------------------------------------------------------------------------------
void SudokuPlayer::UpdateSelectedCell(float deltaSeconds)
{
	UNUSED(deltaSeconds)
	
	// Right mouse button, clear cells
	if (g_input->WasMouseButtonJustPressed(1))
	{
		m_grid->DeselectAllCells();
	}
	
	bool isLeftMouseButtonDown = g_input->IsMouseButtonDown(0);
	if (!isLeftMouseButtonDown)
	{
		return;
	}
	
	// Get cell at the current mouse position
	Vec2 clientRelativePos = g_input->GetMouseClientRelativePosition();
	Vec2 clientCellPos = m_camera->ScreenToWorldOrtho(clientRelativePos);
	IntVec2 cellCoords = IntVec2(FloorF(clientCellPos.x), FloorF(clientCellPos.y)); // round down
	if (!m_grid->IsValidCoords(cellCoords))
	{
		if (g_input->WasMouseButtonJustPressed(0))
		{
			// clicked outside the grid, clear all
			m_grid->DeselectAllCells();
		}
		return;
	}
	
	// Update selection
	if (g_input->WasMouseButtonJustPressed(0))
	{
		// deselect cell if it's already selected, then go into deselect mode
		m_selectMode = !m_grid->IsCellSelected(cellCoords);
		if (!g_input->IsKeyDown(KeyCode::Shift))
		{
			m_grid->DeselectAllCells();
		}
		if (m_selectMode)
		{
			m_grid->SelectCell(cellCoords);
		}
		else
		{
			m_grid->DeselectCell(cellCoords);
		}
	}

	// If dragging, select all cells in the line between last frame's mouse pos and this frame's
	if (isLeftMouseButtonDown)
	{ 
		Vec2 clientCellPosLastFrame = m_camera->ScreenToWorldOrtho(m_mouseClientRelativePosLastFrame);
		SetCellsSelectedInLine(clientCellPos, clientCellPosLastFrame, m_selectMode);
	}
}



//----------------------------------------------------------------------------------------------------------------------
void SudokuPlayer::UpdateArrowKeysSelectedCellMovement(float deltaSeconds)
{
	UNUSED(deltaSeconds)
	
	constexpr float minSecondsBetweenArrowMoves = 0.1f;
	static float timeOfLastArrowMove = 0.f;
	if (GetCurrentTimeSecondsF() - timeOfLastArrowMove > minSecondsBetweenArrowMoves)
	{
		if (g_input->IsKeyDown(KeyCode::Up))
		{
			m_grid->MoveSelectedCell(EDirection::North);
			timeOfLastArrowMove = GetCurrentTimeSecondsF();
		}
		if (g_input->IsKeyDown(KeyCode::Down))
		{
			m_grid->MoveSelectedCell(EDirection::South);
			timeOfLastArrowMove = GetCurrentTimeSecondsF();
		}
		if (g_input->IsKeyDown(KeyCode::Left))
		{
			m_grid->MoveSelectedCell(EDirection::West);
			timeOfLastArrowMove = GetCurrentTimeSecondsF();
		}
		if (g_input->IsKeyDown(KeyCode::Right))
		{
			m_grid->MoveSelectedCell(EDirection::East);
			timeOfLastArrowMove = GetCurrentTimeSecondsF();
		}
	}
}



//----------------------------------------------------------------------------------------------------------------------
void SudokuPlayer::UpdateFill()
{
	if (g_input->WasMouseButtonJustPressed(2))
	{
		m_grid->FillSelectedCells();
	}
}



//----------------------------------------------------------------------------------------------------------------------
void SudokuPlayer::SetCellsSelectedInLine(Vec2 const& start, Vec2 const& end, bool isSelected) const
{
	// Get all cells between last frame's and this frame's mouse positions
	for (int x = 0; x < m_grid->GetWidth(); ++x)
	{
		for (int y = 0; y < m_grid->GetHeight(); ++y)
		{
			AABB2 cellBounds = AABB2((float) x, (float) y, (float) x + 1.f, (float) y + 1.f);
			if (DoesLineIntersectAABB2(start, end, cellBounds))
			{
				if (isSelected)
				{
					m_grid->SelectCell(IntVec2(x, y));
				}
				else
				{
					m_grid->DeselectCell(IntVec2(x, y));
				}
			}
		}
	}
}



//----------------------------------------------------------------------------------------------------------------------
void SudokuPlayer::GetColorPalette() const
{
}



//----------------------------------------------------------------------------------------------------------------------
bool SudokuPlayer::OnCharDown(NamedProperties& args)
{
	if (g_input->IsKeyDown(KeyCode::Ctrl))
	{
		return false;
	}
	
	int c = args.Get("Char", -1);
	m_grid->EnterCharacter((uint8_t)c);
	return true;
}



//----------------------------------------------------------------------------------------------------------------------
bool SudokuPlayer::OnKeyDown(NamedProperties& args)
{
	int key = args.Get("Key", -1);
	if (key == (int) KeyCode::Delete || key == (int) KeyCode::Backspace)
	{
		(*m_grid).ClearSelectedCells();
	}
	if (key == 'Z' && g_input->IsKeyDown(KeyCode::Ctrl))
	{
		m_grid->UndoEvent();
	}
	if (key == 'Y' && g_input->IsKeyDown(KeyCode::Ctrl))
	{
		m_grid->RedoEvent();
	}
	return true;
}



//----------------------------------------------------------------------------------------------------------------------
bool SudokuPlayer::OnMouseWheelUp(NamedProperties& args)
{
	UNUSED(args)
	m_grid->IncrementColorPaletteIndex();
	return true;
}



//----------------------------------------------------------------------------------------------------------------------
bool SudokuPlayer::OnMouseWheelDown(NamedProperties& args)
{
	UNUSED(args)
	m_grid->DecrementColorPaletteIndex();
	return true;
}
