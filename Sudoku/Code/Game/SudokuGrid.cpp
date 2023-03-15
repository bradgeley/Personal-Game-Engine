// Bradley Christensen - 2022-2023
#include "SudokuGrid.h"
#include "Engine/Renderer/VertexBuffer.h"
#include "Engine/Renderer/VertexUtils.h"
#include "Engine/Renderer/Renderer.h"
#include "Engine/Renderer/Font.h"
#include "Engine/Core/EngineCommon.h"
#include "Engine/Core/StringUtils.h"
#include "Engine/Renderer/Window.h"



SudokuGrid::SudokuGrid(SudokuGridConfig const& config) : Grid2D<int>(config.m_startingState), m_config(config), m_selectedCells(config.m_dims, false)
{
	
}



SudokuGrid::~SudokuGrid()
{
	delete m_staticGridVerts;
	delete m_textVerts;
	delete m_selectedCellVerts;
}



void SudokuGrid::Startup()
{
	m_textVerts = new VertexBuffer();
	m_staticGridVerts = new VertexBuffer();
	m_selectedCellVerts = new VertexBuffer();

	auto& verts = m_staticGridVerts->GetMutableVerts();
	AABB2 bounds;
	bounds.mins = Vec2::ZeroVector;
	bounds.maxs = Vec2(m_config.m_dims);
	AddVertsForGrid2D(verts, bounds, m_config.m_dims, 0.033f, Rgba8::LightGray);
	// todo: make boxes a thing?
	AddVertsForGrid2D(verts, bounds, IntVec2(3, 3), 0.08f, Rgba8::Black);
}



void SudokuGrid::Update(float deltaSeconds)
{
	UNUSED(deltaSeconds)
	UpdateTextVerts();
	UpdateSelectedCellVerts();
}



void SudokuGrid::Render() const
{
	g_renderer->DrawVertexBuffer(m_staticGridVerts);
	g_renderer->DrawVertexBuffer(m_selectedCellVerts);

	auto font = g_renderer->GetDefaultFont();
	font->SetRendererState();
	g_renderer->DrawVertexBuffer(m_textVerts);
}



void SudokuGrid::Shutdown()
{
	
}



void SudokuGrid::SelectCell(int index)
{
	if (IsValidIndex(index))
	{
		m_selectedCells.Set(index, true);
	}
}



void SudokuGrid::SelectCell(IntVec2 const& coords)
{
	if (IsValidCoords(coords))
	{
		int cellIndex = GetIndexForCoords(coords);
		m_selectedCells.Set(cellIndex, true);
	}
}



void SudokuGrid::DeselectCell(int index)
{
	if (IsValidIndex(index))
	{
		m_selectedCells.Set(index, false);
	}
}



void SudokuGrid::DeselectCell(IntVec2 const& coords)
{
	if (IsValidCoords(coords))
	{
		int cellIndex = GetIndexForCoords(coords);
		m_selectedCells.Set(cellIndex, false);
	}
}

 

void SudokuGrid::DeselectAllCells()
{
	m_selectedCells.SetAll(false);
}



int SudokuGrid::CountSelectedCells() const
{
	int count = 0;
	for (int i = 0; i < m_selectedCells.Size(); ++i)
	{
		if (m_selectedCells.Get(i))
		{
			++count;
		}
	}
	return count;
}



int SudokuGrid::GetSelectedCellIndex() const
{
	for (int i = 0; i < m_selectedCells.Size(); ++i)
	{
		if (m_selectedCells.Get(i))
		{
			return i;
		}
	}
	return -1;
}



void SudokuGrid::MoveSelectedCell(EDirection direction)
{
	if (CountSelectedCells() != 1)
	{
		return;
	}
	
	int index = GetSelectedCellIndex();
	int nextIndex;
	switch (direction)
	{
		case EDirection::East:
			nextIndex = GetIndexEastOf(index);
			break;
		case EDirection::North:
			nextIndex = GetIndexNorthOf(index);
			break;
		case EDirection::West:
			nextIndex = GetIndexWestOf(index);
			break;
		case EDirection::South:
			nextIndex = GetIndexSouthOf(index);
			break;
		default:
			nextIndex = -1;
			break;
	}

	if (IsValidIndex(nextIndex))
	{
		DeselectCell(index);
		SelectCell(nextIndex);
	}
}



void SudokuGrid::ClearCell()
{
	for (int i = 0; i < m_selectedCells.Size(); ++i)
	{
		if (m_selectedCells.Get(i))
		{
			Set(i, 0);
		}
	}
}



void SudokuGrid::EnterCharacter(uint8_t character)
{
	for (int i = 0; i < m_selectedCells.Size(); ++i)
	{
		if (m_selectedCells.Get(i))
		{
			Set(i, character);
		}
	}
}



void SudokuGrid::UpdateSelectedCellVerts() const
{ 
	g_renderer->BindShader(nullptr);
	g_renderer->BindTexture(nullptr);
	m_selectedCellVerts->ClearVerts();
	for (int i = 0; i < m_selectedCells.Size(); ++i)
	{
		if (!m_selectedCells.Get(i))
		{
			continue;
		}
		AddVertsForSelectedCell(i);
	}
	g_renderer->DrawVertexBuffer(m_selectedCellVerts);
}



void SudokuGrid::AddVertsForSelectedCell(int index) const
{
	IntVec2 coords = GetCoordsForIndex(index);
	auto& verts = m_selectedCellVerts->GetMutableVerts();
	AABB2 selectedCell = AABB2((float) coords.x, (float) coords.y, (float) coords.x + 1.f, (float) coords.y + 1.f);

	Rgba8 backgroundTint = Rgba8(200, 155, 255, 100);
	Rgba8 outlineTint = Rgba8(0, 255, 255, 255);
	float outlineThickness = 0.05f;
	
	// background
	AddVertsForAABB2(verts, selectedCell, backgroundTint);

	// north wall
	int northIndex = GetIndexNorthOf(index);
	bool isNorthBlocked = (northIndex == -1 || !m_selectedCells.Get(northIndex));
	if (isNorthBlocked)
	{
		// blocked to the north, draw the north side of the selection box
		AddVertsForRect2D(verts, selectedCell.GetTopLeft() - Vec2(0.f, outlineThickness), selectedCell.maxs, outlineTint);
	}

	// east wall
	int eastIndex = GetIndexEastOf(index);
	bool isEastBlocked = (eastIndex == -1 || !m_selectedCells.Get(eastIndex));
	if (isEastBlocked)
	{
		// blocked to the north, draw the north side of the selection box
		AddVertsForRect2D(verts, selectedCell.GetBottomRight() - Vec2(outlineThickness, 0.f), selectedCell.maxs, outlineTint);
	}
	
	// south wall
	int southIndex = GetIndexSouthOf(index);
	bool isSouthBlocked = (southIndex == -1 || !m_selectedCells.Get(southIndex));
	if (isSouthBlocked)
	{
		// blocked to the north, draw the north side of the selection box
		AddVertsForRect2D(verts, selectedCell.mins, selectedCell.GetBottomRight() + Vec2(0.f, outlineThickness), outlineTint);
	}

	// west wall
	int westIndex = GetIndexWestOf(index);
	bool isWestBlocked = (westIndex == -1 || !m_selectedCells.Get(westIndex));
	if (isWestBlocked)
	{
		// blocked to the north, draw the north side of the selection box
		AddVertsForRect2D(verts, selectedCell.mins, selectedCell.GetTopLeft() + Vec2(outlineThickness, 0.f), outlineTint);
	}

	// North East Corner (when both are open)
	int northEastIndex = GetIndexEastOf(northIndex);
	bool isNorthEastBlocked = (northEastIndex == -1 || !m_selectedCells.Get(northEastIndex));
	if (!isNorthBlocked && !isEastBlocked && isNorthEastBlocked)
	{
		AddVertsForRect2D(verts, selectedCell.maxs - Vec2(outlineThickness, outlineThickness), selectedCell.maxs, outlineTint);
	}

	// North West Corner (when both are open)
	int northWestIndex = GetIndexWestOf(northIndex);
	bool isNorthWestBlocked = (northWestIndex == -1 || !m_selectedCells.Get(northWestIndex));
	if (!isNorthBlocked && !isWestBlocked && isNorthWestBlocked)
	{
		AddVertsForRect2D(verts, selectedCell.GetTopLeft() - Vec2(0.f, outlineThickness), selectedCell.GetTopLeft() + Vec2(outlineThickness, 0.f), outlineTint);
	}

	// South East Corner (when both are open)
	int southEastIndex = GetIndexEastOf(southIndex);
	bool isSouthEastBlocked = (southEastIndex == -1 || !m_selectedCells.Get(southEastIndex));
	if (!isSouthBlocked && !isEastBlocked && isSouthEastBlocked)
	{
		AddVertsForRect2D(verts, selectedCell.GetBottomRight() - Vec2(outlineThickness, 0.f), selectedCell.GetBottomRight() + Vec2(0.f, outlineThickness), outlineTint);
	}

	// South West Corner (when both are open)
	int southWestIndex = GetIndexWestOf(southIndex);
	bool isSouthWestBlocked = (southWestIndex == -1 || !m_selectedCells.Get(southWestIndex));
	if (!isSouthBlocked && !isWestBlocked && isSouthWestBlocked)
	{
		AddVertsForRect2D(verts, selectedCell.mins, selectedCell.mins + Vec2(outlineThickness, outlineThickness), outlineTint);
	}
}



void SudokuGrid::UpdateTextVerts()
{
	m_textVerts->ClearVerts();
	auto& verts = m_textVerts->GetMutableVerts();
	auto font = g_renderer->GetDefaultFont();

	for (int x = 0; x < GetWidth(); ++x)
	{
		for (int y = 0; y < GetHeight(); ++y)
		{
			int cellIndex = GetIndexForCoords(x, y);
			uint8_t c = static_cast<uint8_t>(m_data[cellIndex]);
			if (c == 0)
			{
				continue;
			}

			Vec2 cellBotLeft = Vec2(x, y);
			std::string asString = StringF("%c", c);
			auto glyphData = font->GetGlyphData(c);
			float glyphWidth = glyphData.m_width;
			float glyphOffsetX = glyphData.a;
			cellBotLeft.x += (1.f - glyphWidth) * 0.5f - glyphOffsetX;
			font->AddVertsForText2D(verts, cellBotLeft, 1.f, asString);
		}
	}
}

