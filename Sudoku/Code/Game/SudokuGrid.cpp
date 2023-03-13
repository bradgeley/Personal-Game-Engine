// Bradley Christensen - 2022-2023
#include "SudokuGrid.h"
#include "Engine/Renderer/VertexBuffer.h"
#include "Engine/Renderer/VertexUtils.cpp"
#include "Engine/Renderer/Camera.cpp"
#include "Engine/Renderer/Renderer.cpp"
#include "Engine/Core/EngineCommon.h"
#include "Engine/Core/StringUtils.h"



SudokuGrid::SudokuGrid(SudokuGridConfig config) : Grid<int>(config.m_startingState), m_config(config)
{
	m_textVerts = new VertexBuffer();
	m_staticGridVerts = new VertexBuffer();
	m_selectedCellVerts = new VertexBuffer();

	auto& verts = m_staticGridVerts->GetMutableVerts();
	AABB2 bounds;
	bounds.mins = Vec2::ZeroVector;
	bounds.maxs = Vec2(m_config.m_dims);
	AddVertsForGrid2D(verts, bounds, GetDimensions(), 0.033f, Rgba8::LightGray);
	// todo: make boxes a thing?
	AddVertsForGrid2D(verts, bounds, IntVec2(3, 3), 0.08f, Rgba8::Black);
}



SudokuGrid::~SudokuGrid()
{
	delete m_staticGridVerts;
	delete m_textVerts;
	delete m_selectedCellVerts;
}



void SudokuGrid::Update(float deltaSeconds)
{
	UNUSED(deltaSeconds)
}



void SudokuGrid::Render() const
{
	g_renderer->DrawVertexBuffer(m_staticGridVerts);
	g_renderer->DrawVertexBuffer(m_selectedCellVerts);

	auto font = g_renderer->GetDefaultFont();
	font->SetRendererState();
	g_renderer->DrawVertexBuffer(m_textVerts);
}



void SudokuGrid::RenderSelectedCells(std::vector<int> const& m_selectedCellIndices) const
{
	m_selectedCellVerts->ClearVerts();
	for (auto& index : m_selectedCellIndices)
	{
		if (IsValidIndex(index))
		{
			IntVec2 coords = GetCoordsForIndex(index);
			auto& verts = m_selectedCellVerts->GetMutableVerts();
			AABB2 selectedCell = AABB2((float) coords.x, (float) coords.y, (float) coords.x + 1.f, (float) coords.y + 1.f);
			float squeeze = 0.05f;
			selectedCell.mins += Vec2(squeeze, squeeze);
			selectedCell.maxs -= Vec2(squeeze, squeeze);
			AddVertsForWireBox2D(verts, selectedCell, 0.1f, Rgba8(100, 100, 200));
		}
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
			if (m_data[cellIndex] == 0)
			{
				// 0 means empty
				continue;
			}

			Vec2 cellBotLeft = Vec2(x, y);
			std::string asString = StringF("%i", m_data[cellIndex]);
			auto glyphData = font->GetGlyphData(asString[0]);
			float glyphWidth = glyphData.m_width;
			float glyphOffsetX = glyphData.a;
			cellBotLeft.x += (1.f - glyphWidth) * 0.5f - glyphOffsetX;
			font->AddVertsForText2D(verts, cellBotLeft, 1.f, asString);
		}
	}
}
