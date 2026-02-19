// Bradley Christensen - 2022-2025
#include "DevConsoleLog.h"
#include "Engine/Math/AABB2.h"
#include "Engine/Math/MathUtils.h"
#include "Engine/Core/StringUtils.h"
#include "Engine/Renderer/Font.h"
#include "Engine/Renderer/Renderer.h"
#include "Engine/Renderer/VertexBuffer.h"
#include <cfloat>



const std::string LINE_PREFIX = "> ";



//----------------------------------------------------------------------------------------------------------------------
void DevConsoleLog::Startup() 
{
    m_vbo = g_renderer->MakeVertexBuffer<Vertex_PCU>();
}



//----------------------------------------------------------------------------------------------------------------------
void DevConsoleLog::Shutdown() 
{
    g_renderer->ReleaseVertexBuffer(m_vbo);
}



//----------------------------------------------------------------------------------------------------------------------
void DevConsoleLog::AddLine(DevConsoleLine const& line)
{
    m_log.push_back(line);
}



//----------------------------------------------------------------------------------------------------------------------
void DevConsoleLog::RenderToBox(AABB2 const& box) const
{
    Font* font = g_renderer->GetDefaultFont(); // todo: let change fonts

    VertexBuffer& vbo = *g_renderer->GetVertexBuffer(m_vbo);
    vbo.ClearVerts();

    float lineThickness = box.GetHeight() / m_numLines;
    float maxLinesOnScreen = box.GetHeight() / lineThickness;

    float linesRendered = 0.f;
    int lineIndex = (int) m_log.size() - 1 - (int) m_scrollOffset;
    while (linesRendered < maxLinesOnScreen && lineIndex >= 0)
    {
        float yOffsetBot = linesRendered * lineThickness;
        float yOffsetTop = (linesRendered + 1.f) * lineThickness;
        AABB2 textBox = AABB2(box.mins.x, box.mins.y + yOffsetBot, box.maxs.x, box.mins.y + yOffsetTop);
        float squeeze = textBox.GetHeight() / 15.f;
        textBox.Squeeze(squeeze);
        font->AddVertsForText2D(vbo, textBox.mins, textBox.GetHeight(), LINE_PREFIX + m_log[lineIndex].m_line, m_log[lineIndex].m_tint);
        
        --lineIndex;
        linesRendered += 1.f;
    }

    font->SetRendererState();
    g_renderer->DrawVertexBuffer(vbo);
}



//----------------------------------------------------------------------------------------------------------------------
void DevConsoleLog::Scroll(int scrollAmount)
{
    m_scrollOffset += (float) scrollAmount;
    float numLines = (float) m_log.size();
    float maxLines = m_numLines;
    float maxOffset = numLines - maxLines + 1.f;
    maxOffset = MathUtils::Clamp(maxOffset, 0.f, FLT_MAX);
    m_scrollOffset = MathUtils::Clamp(m_scrollOffset, 0.f, maxOffset);
}



//----------------------------------------------------------------------------------------------------------------------
void DevConsoleLog::ScrollToTop()
{
    float numLines = (float) m_log.size();
    float maxLines = m_numLines;
    float maxOffset = numLines - maxLines + 1.f;
    maxOffset = MathUtils::Clamp(maxOffset, 0.f, FLT_MAX);
    m_scrollOffset = maxOffset;
}



//----------------------------------------------------------------------------------------------------------------------
void DevConsoleLog::ScrollToBottom()
{
    m_scrollOffset = 0.f;
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
