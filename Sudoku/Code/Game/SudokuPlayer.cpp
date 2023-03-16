// Bradley Christensen - 2022-2023
#include "Engine/Renderer/Camera.h"
#include "Engine/Input/InputSystem.h"
#include "Engine/Core/Time.h"
#include "Engine/Math/Grid2D.h"
#include "Engine/Core/EngineCommon.h"
#include "Engine/DataStructures/NamedProperties.h"
#include "SudokuPlayer.h"
#include "SudokuGrid.h"
#include "Engine/Debug/DebugDrawUtils.h"
#include "Engine/Math/GeometryUtils.h"



class VertexBuffer;



SudokuPlayer::SudokuPlayer()
{
	m_camera = new Camera();
}



SudokuPlayer::~SudokuPlayer()
{
	 
}



void SudokuPlayer::Startup()
{
	g_input->m_charInputEvent.SubscribeMethod(this, &SudokuPlayer::OnCharDown);
	g_input->m_keyDownEvent.SubscribeMethod(this, &SudokuPlayer::OnKeyDown);
}



void SudokuPlayer::Update(float deltaSeconds)
{
	UpdateSelectedCell(deltaSeconds);
	UpdateArrowKeysSelectedCellMovement(deltaSeconds);
}



void SudokuPlayer::Render() const
{
	// Render the drag line
	Vec2 clientRelativePos = g_input->GetMouseClientRelativePosition();
	Vec2 clientCellPos = m_camera->ScreenToWorldOrtho(clientRelativePos);
	Vec2 clientCellPosLastFrame = m_camera->ScreenToWorldOrtho(m_mouseClientRelativePosLastFrame);
	DebugDrawLine2D(clientCellPos, clientCellPosLastFrame, 0.1f, Rgba8::Green);
}



void SudokuPlayer::EndFrame()
{
	m_mouseClientRelativePosLastFrame = g_input->GetMouseClientRelativePosition();
}



void SudokuPlayer::Shutdown()
{
	g_input->m_charInputEvent.UnsubscribeMethod(this, &SudokuPlayer::OnCharDown);
	g_input->m_keyDownEvent.UnsubscribeMethod(this, &SudokuPlayer::OnKeyDown);
}



void SudokuPlayer::BeginGame(SudokuGrid* grid)
{
	m_grid = grid;
	Vec3 boardMins = Vec3::ZeroVector;
	Vec3 boardMaxs = Vec3(grid->GetDimensions(), 1.f);
	m_camera->SetOrthoBounds(boardMins - Vec3(1.f, 1.f), boardMaxs + Vec3(1.f, 1.f));
}



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
	IntVec2 cellCoords = IntVec2(clientCellPos); // round down
	if (!m_grid->IsValidCoords(cellCoords))
	{
		return;
	}
	
	// Update selection
	if (g_input->WasMouseButtonJustPressed(0))
	{
		if (!g_input->IsKeyDown(KeyCode::SHIFT))
		{
			m_grid->DeselectAllCells();
		}
		m_grid->SelectCell(cellCoords);
	}

	// If dragging, select all cells in the line between last frame's mouse pos and this frame's
	if (isLeftMouseButtonDown)
	{ 
		Vec2 clientCellPosLastFrame = m_camera->ScreenToWorldOrtho(m_mouseClientRelativePosLastFrame);
		SelectCellsInLine(clientCellPos, clientCellPosLastFrame);
	}
}



void SudokuPlayer::UpdateArrowKeysSelectedCellMovement(float deltaSeconds)
{
	UNUSED(deltaSeconds)
	
	constexpr float minSecondsBetweenArrowMoves = 0.1f;
	static float timeOfLastArrowMove = 0.f;
	if (GetCurrentTimeSecondsF() - timeOfLastArrowMove > minSecondsBetweenArrowMoves)
	{
		if (g_input->IsKeyDown(KeyCode::UP))
		{
			m_grid->MoveSelectedCell(EDirection::North);
			timeOfLastArrowMove = GetCurrentTimeSecondsF();
		}
		if (g_input->IsKeyDown(KeyCode::DOWN))
		{
			m_grid->MoveSelectedCell(EDirection::South);
			timeOfLastArrowMove = GetCurrentTimeSecondsF();
		}
		if (g_input->IsKeyDown(KeyCode::LEFT))
		{
			m_grid->MoveSelectedCell(EDirection::West);
			timeOfLastArrowMove = GetCurrentTimeSecondsF();
		}
		if (g_input->IsKeyDown(KeyCode::RIGHT))
		{
			m_grid->MoveSelectedCell(EDirection::East);
			timeOfLastArrowMove = GetCurrentTimeSecondsF();
		}
	}
}



void SudokuPlayer::SelectCellsInLine(Vec2 const& start, Vec2 const& end) const
{
	// Get all cells between last frame's and this frame's mouse positions
	for (int x = 0; x < m_grid->GetWidth(); ++x)
	{
		for (int y = 0; y < m_grid->GetHeight(); ++y)
		{
			AABB2 cellBounds = AABB2((float) x, (float) y, (float) x + 1.f, (float) y + 1.f);
			if (DoesLineIntersectAABB2(start, end, cellBounds))
			{
				m_grid->SelectCell(IntVec2(x, y));
			}
		}
	}
}



bool SudokuPlayer::OnCharDown(NamedProperties& args)
{
	int c = args.Get("Char", -1);
	m_grid->EnterCharacter((uint8_t)c);
	return true;
}



bool SudokuPlayer::OnKeyDown(NamedProperties& args)
{
	int key = args.Get("Key", -1);
	if (key == (int) KeyCode::DEL || key == (int) KeyCode::BACKSPACE)
	{
		(*m_grid).ClearCell();
	}
	return true;
}
