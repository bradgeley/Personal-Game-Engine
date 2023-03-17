// Bradley Christensen - 2022-2023
#include "SudokuGrid.h"

#include "SudokuHistoryEvent.h"
#include "Engine/Renderer/VertexBuffer.h"
#include "Engine/Renderer/VertexUtils.h"
#include "Engine/Renderer/Renderer.h"
#include "Engine/Renderer/Font.h"
#include "Engine/Core/EngineCommon.h"
#include "Engine/Core/StringUtils.h"
#include "Engine/Math/MathUtils.h"
#include "Engine/Renderer/Window.h"



SudokuGrid::SudokuGrid(SudokuGridConfig const& config) :
	Grid2D<int>(config.m_startingState),
	m_config(config),
	m_selectedCells(config.m_dims, false),
	m_cellShading(config.m_cellShading)
{
	
}



SudokuGrid::~SudokuGrid()
{
	delete m_staticGridVerts;
	delete m_textVerts;
	delete m_selectedCellVerts;
	delete m_cellShadingVerts;
}



void SudokuGrid::Startup()
{
	m_textVerts = new VertexBuffer();
	m_staticGridVerts = new VertexBuffer();
	m_selectedCellVerts = new VertexBuffer();
	m_cellShadingVerts = new VertexBuffer();

	auto& staticGridVerts = m_staticGridVerts->GetMutableVerts();
	AABB2 bounds;
	bounds.mins = Vec2::ZeroVector;
	bounds.maxs = Vec2(m_config.m_dims);
	AddVertsForWireGrid2D(staticGridVerts, bounds, m_config.m_dims, m_config.m_gridLineThickness, m_config.m_smallGridLineColor);
	// todo: make boxes a thing?
	AddVertsForWireGrid2D(staticGridVerts, bounds, IntVec2(3, 3), m_config.m_regionLineThickness, m_config.m_bigGridLineColor);

	auto& cellShadingVerts = m_cellShadingVerts->GetMutableVerts();
	AddVertsForGrid2D(cellShadingVerts, bounds, m_config.m_dims);

	// init cell shading from config
	for (int i = 0; i < m_config.m_cellShading.Size(); ++i)
	{
		SetCellFill(i, m_cellShading.Get(i));
	}
}



void SudokuGrid::Update(float deltaSeconds)
{
	UNUSED(deltaSeconds)
	UpdateTextVerts();
	UpdateSelectedCellVerts();
}



void SudokuGrid::Render() const
{
	g_renderer->ClearScreen(m_config.m_backgroundColor);
	
	g_renderer->BindShader(nullptr);
	g_renderer->BindTexture(nullptr);
	g_renderer->DrawVertexBuffer(m_cellShadingVerts);
	g_renderer->DrawVertexBuffer(m_staticGridVerts);
	g_renderer->DrawVertexBuffer(m_selectedCellVerts);

	Vec2 colorPaletteOrigin = Vec2(-m_config.m_colorPaletteScale - 0.1f, 0.f);
	RenderColorPalette(colorPaletteOrigin);

	auto font = g_renderer->GetDefaultFont();
	font->SetRendererState();
	g_renderer->SetFontConstants(m_config.m_bigDigitFontConstants);
	g_renderer->DrawVertexBuffer(m_textVerts);
	g_renderer->SetFontConstants(FontConstants());
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
		if (IsCellSelected(i))
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
		if (IsCellSelected(i))
		{
			return i;
		}
	}
	return -1;
}



bool SudokuGrid::IsCellSelected(int index) const
{
	return m_selectedCells.Get(index);
}



bool SudokuGrid::IsGivenDigit(int index) const
{
	return m_config.m_startingState.Get(index) != 0;
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



void SudokuGrid::ClearSelectedCells()
{
	EnterCharacter(0);
}



void SudokuGrid::EnterCharacter(uint8_t character)
{
	SudokuEventChangeDigits* event = new SudokuEventChangeDigits();
	for (int i = 0; i < m_selectedCells.Size(); ++i)
	{
		if (IsCellSelected(i) && !IsGivenDigit(i))
		{
			event->m_indices.push_back((uint8_t)i);
			event->m_digitsBefore.push_back((uint8_t)Get(i));
			Set(i, character);
			event->m_digitsAfter.push_back(character);
		}
	}
	m_history.AddEvent(event);
}



void SudokuGrid::FillSelectedCells()
{
	Rgba8& tint = m_config.m_colorPalette[m_currentColorPaletteIndex];
	
	SudokuEventFillColor* event = new SudokuEventFillColor();
	for (int i = 0; i < m_selectedCells.Size(); ++i)
	{
		if (IsCellSelected(i))
		{
			event->m_indices.push_back((uint8_t)i);
			event->m_tintBefore.push_back(m_cellShading.Get(i));
			SetCellFill(i, tint);
			event->m_tintAfter.push_back(tint);
		}
	}
	m_history.AddEvent(event);
}



void SudokuGrid::SetCellFill(int index, Rgba8 const& tint)
{
	m_cellShading.Set(index, tint);
	// Update the grid representation of the cell shading

	auto& verts = m_cellShadingVerts->GetMutableVerts();
	int vertIndex = index * 6; // 6 verts per rect, in index order
	for (int i = vertIndex; i < vertIndex + 6; ++i)
	{
		verts[i].tint = tint;
	}
}



void SudokuGrid::SetCellCharacter(int index, uint8_t character)
{
	Set(index, character);
}



void SudokuGrid::IncrementColorPaletteIndex()
{
	m_currentColorPaletteIndex = IncrementIntInRange(m_currentColorPaletteIndex, 0, (int) m_config.m_colorPalette.size() - 1, true);
}



void SudokuGrid::DecrementColorPaletteIndex()
{
	m_currentColorPaletteIndex = DecrementIntInRange(m_currentColorPaletteIndex, 0, (int) m_config.m_colorPalette.size() - 1, true);
}



void SudokuGrid::UndoEvent()
{
	m_history.Undo(this);
}



void SudokuGrid::RedoEvent()
{
	m_history.Redo(this);
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
			Rgba8 tint = IsGivenDigit(cellIndex) ? m_config.m_givenDigitColor : m_config.m_enteredDigitColor;
			font->AddVertsForText2D(verts, cellBotLeft, 1.f, asString, tint);
		}
	}
}



void SudokuGrid::UpdateSelectedCellVerts() const
{ 
	m_selectedCellVerts->ClearVerts();
	for (int i = 0; i < m_selectedCells.Size(); ++i)
	{
		if (!IsCellSelected(i))
		{
			continue;
		}
		AddVertsForSelectedCell(i);
	}
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
	bool isNorthBlocked = (northIndex == -1 || !IsCellSelected(northIndex));
	if (isNorthBlocked)
	{
		// blocked to the north, draw the north side of the selection box
		AddVertsForRect2D(verts, selectedCell.GetTopLeft() - Vec2(0.f, outlineThickness), selectedCell.maxs, outlineTint);
	}

	// east wall
	int eastIndex = GetIndexEastOf(index);
	bool isEastBlocked = (eastIndex == -1 || !IsCellSelected(eastIndex));
	if (isEastBlocked)
	{
		// blocked to the north, draw the north side of the selection box
		AddVertsForRect2D(verts, selectedCell.GetBottomRight() - Vec2(outlineThickness, 0.f), selectedCell.maxs, outlineTint);
	}
	
	// south wall
	int southIndex = GetIndexSouthOf(index);
	bool isSouthBlocked = (southIndex == -1 || !IsCellSelected(southIndex));
	if (isSouthBlocked)
	{
		// blocked to the north, draw the north side of the selection box
		AddVertsForRect2D(verts, selectedCell.mins, selectedCell.GetBottomRight() + Vec2(0.f, outlineThickness), outlineTint);
	}

	// west wall
	int westIndex = GetIndexWestOf(index);
	bool isWestBlocked = (westIndex == -1 || !IsCellSelected(westIndex));
	if (isWestBlocked)
	{
		// blocked to the north, draw the north side of the selection box
		AddVertsForRect2D(verts, selectedCell.mins, selectedCell.GetTopLeft() + Vec2(outlineThickness, 0.f), outlineTint);
	}

	// North East Corner (when both are open)
	int northEastIndex = GetIndexEastOf(northIndex);
	bool isNorthEastBlocked = (northEastIndex == -1 || !IsCellSelected(northEastIndex));
	if (!isNorthBlocked && !isEastBlocked && isNorthEastBlocked)
	{
		AddVertsForRect2D(verts, selectedCell.maxs - Vec2(outlineThickness, outlineThickness), selectedCell.maxs, outlineTint);
	}

	// North West Corner (when both are open)
	int northWestIndex = GetIndexWestOf(northIndex);
	bool isNorthWestBlocked = (northWestIndex == -1 || !IsCellSelected(northWestIndex));
	if (!isNorthBlocked && !isWestBlocked && isNorthWestBlocked)
	{
		AddVertsForRect2D(verts, selectedCell.GetTopLeft() - Vec2(0.f, outlineThickness), selectedCell.GetTopLeft() + Vec2(outlineThickness, 0.f), outlineTint);
	}

	// South East Corner (when both are open)
	int southEastIndex = GetIndexEastOf(southIndex);
	bool isSouthEastBlocked = (southEastIndex == -1 || !IsCellSelected(southEastIndex));
	if (!isSouthBlocked && !isEastBlocked && isSouthEastBlocked)
	{
		AddVertsForRect2D(verts, selectedCell.GetBottomRight() - Vec2(outlineThickness, 0.f), selectedCell.GetBottomRight() + Vec2(0.f, outlineThickness), outlineTint);
	}

	// South West Corner (when both are open)
	int southWestIndex = GetIndexWestOf(southIndex);
	bool isSouthWestBlocked = (southWestIndex == -1 || !IsCellSelected(southWestIndex));
	if (!isSouthBlocked && !isWestBlocked && isSouthWestBlocked)
	{
		AddVertsForRect2D(verts, selectedCell.mins, selectedCell.mins + Vec2(outlineThickness, outlineThickness), outlineTint);
	}
}



void SudokuGrid::RenderColorPalette(Vec2 const& origin) const
{
	VertexBuffer paletteVbo, textVbo;
	auto& paletteVerts = paletteVbo.GetMutableVerts();
	
	for (int i = 0; i < (int) m_config.m_colorPalette.size(); ++i)
	{
		Rgba8 const& tint = m_config.m_colorPalette[i];
		Vec2 botLeft = origin + Vec2(0.f, (float) i * m_config.m_colorPaletteScale);
		Vec2 topRight = botLeft + Vec2(m_config.m_colorPaletteScale, m_config.m_colorPaletteScale);
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

