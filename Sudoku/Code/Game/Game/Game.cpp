// Bradley Christensen - 2022-2023
#include "Game.h"

#include "GameCommon.h"
#include "WindowsApplication.h"
#include "Engine/Input/InputSystem.h"
#include "Engine/Input/InputUtils.h"
#include "Engine/Math/MathUtils.h"
#include "Engine/Renderer/DebugDrawUtils.h"
#include "Engine/Renderer/Font.h"
#include "Engine/Renderer/Renderer.h"
#include "Engine/Renderer/Texture.h"
#include "Engine/Renderer/VertexBuffer.h"
#include "Engine/Renderer/VertexUtils.h"
#include <Engine\Core\EngineCommon.h>
#include <Engine\Core\StringUtils.h>
#include <Engine\Core\Time.h>




void Game::Startup()
{
    m_camera = Camera(Vec3(0.f, 0.f, 0.f), Vec3(9.f, 9.f, 1.f));

    m_grid = Grid<int>(IntVec2(9, 9), 0);
    for (int i = 0; i < 10; ++i)
    {
        m_grid[i] = i;
    }

    // Make the grid
    m_staticGridVerts = new VertexBuffer();
    auto& verts = m_staticGridVerts->GetMutableVerts();
	AddVertsForGrid2D(verts, m_camera.GetOrthoBounds2D(), m_grid.GetDimensions(), 0.033f, Rgba8::LightGray);
	AddVertsForGrid2D(verts, m_camera.GetOrthoBounds2D(), IntVec2(3, 3), 0.08f, Rgba8::Black);

    // Make the number verts
    m_textVerts = new VertexBuffer();
    m_selectedCellVerts = new VertexBuffer();
}



void Game::Update(float deltaSeconds)
{
    UNUSED(deltaSeconds)
    if (g_input->WasKeyJustReleased(KeyCode::ESCAPE))
    {
        g_app->Quit();
    }

    UpdateTextVerts();
    UpdateSelectedCell(deltaSeconds);
    UpdateNumberEntry();
}

 

void Game::Render() const
{
    g_renderer->BeginCamera(m_camera); // reset renderer state

    // Clear screen first
    g_renderer->ClearScreen(Rgba8::White);

	g_renderer->DrawVertexBuffer(m_staticGridVerts);
	g_renderer->DrawVertexBuffer(m_selectedCellVerts);
  
	auto font = g_renderer->GetDefaultFont();
    font->SetRendererState();
    g_renderer->DrawVertexBuffer(m_textVerts);
}



void Game::Shutdown()
{
    delete m_staticGridVerts;
    delete m_textVerts;
    delete m_selectedCellVerts;
}



void Game::UpdateTextVerts()
{
    m_textVerts->ClearVerts();
    auto& verts = m_textVerts->GetMutableVerts();
    auto font = g_renderer->GetDefaultFont();

    for (int x = 0; x < m_grid.GetWidth(); ++x)
    {
        for (int y = 0; y < m_grid.GetHeight(); ++y)
		{
			int cellIndex = m_grid.GetIndexForCoords(x, y);
            if (m_grid[cellIndex] == 0)
            {
                // 0 means empty
                continue;
            }

            Vec2 cellBotLeft = Vec2(x, y);
			std::string asString = StringF("%i", m_grid[cellIndex]);
            auto glyphData = font->GetGlyphData(asString[0]);
            float glyphWidth = glyphData.m_width;
            cellBotLeft.x += (1.f - glyphWidth) * 0.5f - glyphData.a;
            font->AddVertsForText2D(verts, cellBotLeft, 1.f, asString);
        }
    }
}



void Game::UpdateSelectedCell(float deltaSeconds)
{
    // Update selection
    bool selectionChanged = false;
    if (g_input->WasMouseButtonJustPressed(0))
    {
        Vec2 clientRelativePos = g_input->GetMouseClientRelativePosition();
        Vec2 clientCellPos = clientRelativePos * Vec2(m_grid.GetWidth(), m_grid.GetHeight());
        IntVec2 cellCoords = IntVec2(clientCellPos); // round down
        int cellIndex = m_grid.GetIndexForCoords(cellCoords);
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

	IntVec2 selectedCellCoords = m_grid.GetCoordsForIndex(m_selectedCellIndex);

    constexpr float minSecondsBetweenArrowMoves = 0.1f;
    static float timeOfLastArrowMove = 0.f;
    if (GetCurrentTimeSecondsF() - timeOfLastArrowMove > minSecondsBetweenArrowMoves)
    {
		if (g_input->IsKeyDown(KeyCode::UP))
		{
			selectedCellCoords.y++;
			if (selectedCellCoords.y >= m_grid.GetHeight())
			{
				selectedCellCoords.y = 0;
			}
			m_selectedCellIndex = m_grid.GetIndexForCoords(selectedCellCoords);
			selectionChanged = true;
			timeOfLastArrowMove = GetCurrentTimeSecondsF();
		}
		if (g_input->IsKeyDown(KeyCode::DOWN))
		{
			selectedCellCoords.y--;
			if (selectedCellCoords.y < 0.f)
			{
				selectedCellCoords.y = m_grid.GetHeight() - 1;
			}
			m_selectedCellIndex = m_grid.GetIndexForCoords(selectedCellCoords);
			selectionChanged = true;
			timeOfLastArrowMove = GetCurrentTimeSecondsF();
		}
		if (g_input->IsKeyDown(KeyCode::LEFT))
		{
			selectedCellCoords.x--;
			if (selectedCellCoords.x < 0.f)
			{
				selectedCellCoords.x = m_grid.GetWidth() - 1;
			}
			m_selectedCellIndex = m_grid.GetIndexForCoords(selectedCellCoords);
			selectionChanged = true;
			timeOfLastArrowMove = GetCurrentTimeSecondsF();
		}
		if (g_input->IsKeyDown(KeyCode::RIGHT))
		{
			selectedCellCoords.x++;
			if (selectedCellCoords.x >= m_grid.GetWidth())
			{
				selectedCellCoords.x = 0;
			}
			m_selectedCellIndex = m_grid.GetIndexForCoords(selectedCellCoords);
			selectionChanged = true;
			timeOfLastArrowMove = GetCurrentTimeSecondsF();
		}
    }


    if (selectionChanged)
    {
		// Update verts
		m_selectedCellVerts->ClearVerts();
        if (m_selectedCellIndex > -1)
		{
			auto& verts = m_selectedCellVerts->GetMutableVerts();
			AABB2 selectedCell = AABB2((float) selectedCellCoords.x, (float) selectedCellCoords.y, (float) selectedCellCoords.x + 1.f, (float) selectedCellCoords.y + 1.f);
			float squeeze = 0.05f;
			selectedCell.mins += Vec2(squeeze, squeeze);
			selectedCell.maxs -= Vec2(squeeze, squeeze);
			AddVertsForWireBox2D(verts, selectedCell, 0.1f, Rgba8(100,100,200));
        }
    }
}



void Game::UpdateNumberEntry()
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
        m_grid[m_selectedCellIndex] = numEntered;
    }

    if (g_input->IsKeyDown(KeyCode::DEL) || g_input->IsKeyDown(KeyCode::BACKSPACE))
	{
        m_grid[m_selectedCellIndex] = 0;
    }
}