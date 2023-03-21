﻿// Bradley Christensen - 2023
#include "DevConsoleLog.h"
#include "Engine/Math/AABB2.h"
#include "Engine/Math/MathUtils.h"
#include "Engine/Renderer/Font.h"
#include "Engine/Renderer/Renderer.h"
#include "Engine/Renderer/VertexBuffer.h"



//----------------------------------------------------------------------------------------------------------------------
void DevConsoleLog::AddLine(DevConsoleLine const& line)
{
    m_log.emplace_back(line);
}



//----------------------------------------------------------------------------------------------------------------------
void DevConsoleLog::RenderToBox(AABB2 const& box) const
{
    Font* font = g_renderer->GetDefaultFont(); // todo: let change fonts

    VertexBuffer buffer;
    auto& verts = buffer.GetMutableVerts();

    float lineThickness = box.GetHeight() / m_numLines;

    float linesRendered = 0.f;
    int lineIndex = (int) m_log.size() - 1 - m_scrollOffset;
    while (lineIndex >= 0)
    {
        float yOffsetBot = linesRendered * lineThickness;
        float yOffsetTop = (linesRendered + 1.f) * lineThickness;
        AABB2 textBox = AABB2(box.mins.x, box.mins.y + yOffsetBot, box.maxs.x, box.mins.y + yOffsetTop);
        float squeeze = textBox.GetHeight() / 15.f;
        textBox.Squeeze(squeeze);
        font->AddVertsForText2D(verts, textBox.mins, textBox.GetHeight(), "> " + m_log[lineIndex].m_line, m_log[lineIndex].m_tint);
        
        --lineIndex;
        linesRendered += 1.f;
    }

    font->SetRendererState();
    g_renderer->DrawVertexBuffer(&buffer);
}



//----------------------------------------------------------------------------------------------------------------------
void DevConsoleLog::Scroll(int scrollAmount)
{
    m_scrollOffset += (float) scrollAmount;
    float numLines = (float) m_log.size();
    float maxLines = m_numLines;
    float maxOffset = numLines - maxLines;
    maxOffset = ClampF(maxOffset, 0, INT_MAX);
    m_scrollOffset = ClampF(m_scrollOffset, 0, maxOffset);
}



//----------------------------------------------------------------------------------------------------------------------
void DevConsoleLog::SetNumLines(float numLines)
{
    m_numLines = numLines;
}



//----------------------------------------------------------------------------------------------------------------------
void DevConsoleLog::Clear()
{
    m_scrollOffset = 0.f;
    m_log.clear();
}
