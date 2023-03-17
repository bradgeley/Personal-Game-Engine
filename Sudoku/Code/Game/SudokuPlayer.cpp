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
#include "Engine/Math/MathUtils.h"
#include "Engine/Renderer/Renderer.h"
#include "Engine/Renderer/VertexBuffer.h"
#include "Engine/Renderer/VertexUtils.h"



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
	g_input->m_mouseWheelUpEvent.SubscribeMethod(this, &SudokuPlayer::OnMouseWheelUp);
	g_input->m_mouseWheelDownEvent.SubscribeMethod(this, &SudokuPlayer::OnMouseWheelDown);

	m_colorPalette.emplace_back(Rgba8(158, 1, 66));
	m_colorPalette.emplace_back(Rgba8(213, 62, 79));
	m_colorPalette.emplace_back(Rgba8(244, 109, 67));
	m_colorPalette.emplace_back(Rgba8(253, 174, 97));
	m_colorPalette.emplace_back(Rgba8(254, 224, 139));
	m_colorPalette.emplace_back(Rgba8(230, 245, 152));
	m_colorPalette.emplace_back(Rgba8(171, 221, 164));
	m_colorPalette.emplace_back(Rgba8(102, 194, 165));
	//m_colorPalette.emplace_back(50, 136, 189); blue
	m_colorPalette.emplace_back(Rgba8(94, 79, 162));
	m_colorPalette.emplace_back(Rgba8::White);
}



void SudokuPlayer::Update(float deltaSeconds)
{
	UpdateSelectedCell(deltaSeconds);
	UpdateArrowKeysSelectedCellMovement(deltaSeconds);
	UpdateFill();
}



void SudokuPlayer::Render() const
{
	RenderColorPalette();
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
	IntVec2 cellCoords = IntVec2(FloorF(clientCellPos.x), FloorF(clientCellPos.y)); // round down
	if (!m_grid->IsValidCoords(cellCoords))
	{
		if (g_input->WasMouseButtonJustPressed(0))
		{
			// clicked outside the grid
			m_grid->DeselectAllCells();
		}
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



void SudokuPlayer::UpdateFill()
{
	if (g_input->WasMouseButtonJustPressed(2))
	{
		m_grid->FillSelectedCells(m_colorPalette[m_currentColorPaletteIndex]);
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



void SudokuPlayer::RenderColorPalette() const
{
	VertexBuffer paletteVbo, textVbo;
	auto& paletteVerts = paletteVbo.GetMutableVerts();
	
	Vec2 botLeftOfScreen = m_camera->ScreenToWorldOrtho(Vec2::ZeroVector);
	
	for (int i = 0; i < (int) m_colorPalette.size(); ++i)
	{
		Rgba8 const& tint = m_colorPalette[i];
		Vec2 botLeft = botLeftOfScreen + Vec2(0.f, (float) i * 0.5f);
		Vec2 topRight = botLeft + Vec2(0.5f, 0.5f);
		AABB2 box(botLeft, topRight);
		AddVertsForAABB2(paletteVerts, box, tint);

		if (m_currentColorPaletteIndex == i)
		{
			// Draw thick yellow outline
			box.Squeeze(0.02f);
			AddVertsForWireBox2D(paletteVerts, box, 0.04f, Rgba8::Yellow);
		}
		else
		{
			// Draw thin black outline
			box.Squeeze(0.015f);
			AddVertsForWireBox2D(paletteVerts, box, 0.03f, Rgba8::Black);
		}
	}

	g_renderer->BindShader(nullptr);
	g_renderer->BindTexture(nullptr);
	g_renderer->DrawVertexBuffer(&paletteVbo);
}



bool SudokuPlayer::OnCharDown(NamedProperties& args)
{
	if (g_input->IsKeyDown(KeyCode::CTRL))
	{
		return false;
	}
	
	int c = args.Get("Char", -1);
	m_grid->EnterCharacter((uint8_t)c);
	return true;
}



bool SudokuPlayer::OnKeyDown(NamedProperties& args)
{
	int key = args.Get("Key", -1);
	if (key == (int) KeyCode::DEL || key == (int) KeyCode::BACKSPACE)
	{
		(*m_grid).ClearSelectedCells();
	}
	if (key == 'Z' && g_input->IsKeyDown(KeyCode::CTRL))
	{
		m_grid->RevertLastEvent();
	}
	if (key == 'Y' && g_input->IsKeyDown(KeyCode::CTRL))
	{
		m_grid->RestoreLastEvent();
	}
	return true;
}



bool SudokuPlayer::OnMouseWheelUp(NamedProperties& args)
{
	UNUSED(args)
	m_currentColorPaletteIndex = IncrementIntInRange(m_currentColorPaletteIndex, 0, (int) m_colorPalette.size() - 1, true);
	return true;
}



bool SudokuPlayer::OnMouseWheelDown(NamedProperties& args)
{
	UNUSED(args)
	m_currentColorPaletteIndex = DecrementIntInRange(m_currentColorPaletteIndex, 0, (int) m_colorPalette.size() - 1, true);
	return true;
}
