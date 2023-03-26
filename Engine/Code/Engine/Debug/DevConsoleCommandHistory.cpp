// Bradley Christensen - 2023
#include "DevConsoleCommandHistory.h"
#include "Engine/Math/AABB2.h"
#include "Engine/Math/MathUtils.h"
#include "Engine/Renderer/Font.h"
#include "Engine/Renderer/Renderer.h"
#include "Engine/Renderer/VertexBuffer.h"
#include "Engine/Renderer/VertexUtils.h"



//----------------------------------------------------------------------------------------------------------------------
void DevConsoleCommandHistory::OnCommandEntered(std::string const& command)
{
    m_log.emplace_back(command);
}



//----------------------------------------------------------------------------------------------------------------------
std::string DevConsoleCommandHistory::GetSelectedCommand() const
{
    if (m_selectedLineIndex == -1)
    {
        return "";
    }
    return m_log[m_selectedLineIndex];
}



//----------------------------------------------------------------------------------------------------------------------
bool DevConsoleCommandHistory::IsActive() const
{
    return m_selectedLineIndex != -1;
}



//----------------------------------------------------------------------------------------------------------------------
void DevConsoleCommandHistory::RenderToBox(AABB2 const& box) const
{
    Font* font = g_renderer->GetDefaultFont(); // todo: let change fonts

    // Draw Background
    VertexBuffer backgroundBuffer;
    auto& bgVerts = backgroundBuffer.GetMutableVerts();
    AddVertsForAABB2(bgVerts, box, Rgba8(0, 0, 0, 200));
    AddVertsForWireBox2D(bgVerts, box, 0.0025f, Rgba8(255, 255, 255, 200));

    g_renderer->BindShader(nullptr);
    g_renderer->BindTexture(nullptr);
    g_renderer->DrawVertexBuffer(&backgroundBuffer);
    backgroundBuffer.ClearVerts();
    //

    VertexBuffer buffer;
    auto& verts = buffer.GetMutableVerts();

    float lineThickness = box.GetHeight() / m_numLines;

    float linesRendered = 0.f;
    int lineIndex = (int) m_log.size() - 1 - (int) m_scrollOffset;
    while (lineIndex >= 0)
    {
        float yOffsetBot = linesRendered * lineThickness;
        float yOffsetTop = (linesRendered + 1.f) * lineThickness;
        AABB2 textBox = AABB2(box.mins.x, box.mins.y + yOffsetBot, box.maxs.x, box.mins.y + yOffsetTop);
        float squeeze = textBox.GetHeight() / 15.f;
        textBox.Squeeze(squeeze);
        
        if (m_selectedLineIndex == lineIndex)
        {
            AddVertsForAABB2(bgVerts, textBox, Rgba8::Cerulean);
            g_renderer->BindShader(nullptr);
            g_renderer->BindTexture(nullptr);
            g_renderer->DrawVertexBuffer(&backgroundBuffer);
        }
        
        font->AddVertsForText2D(verts, textBox.mins, textBox.GetHeight(), "> " + m_log[lineIndex], Rgba8::White);
        
        --lineIndex;
        linesRendered += 1.f;
    }

    font->SetRendererState();
    g_renderer->DrawVertexBuffer(&buffer);
}



//----------------------------------------------------------------------------------------------------------------------
void DevConsoleCommandHistory::ArrowUp()
{
    m_selectedLineIndex = DecrementIntInRange(m_selectedLineIndex, -1, (int) m_log.size() - 1, true);
}



//----------------------------------------------------------------------------------------------------------------------
void DevConsoleCommandHistory::ArrowDown()
{
    m_selectedLineIndex = IncrementIntInRange(m_selectedLineIndex, -1, (int) m_log.size() - 1, true);
}



//----------------------------------------------------------------------------------------------------------------------
void DevConsoleCommandHistory::Scroll(int scrollAmount)
{
    m_scrollOffset += (float) scrollAmount;
    float numLines = (float) m_log.size();
    float maxLines = m_numLines;
    float maxOffset = numLines - maxLines;
    maxOffset = ClampF(maxOffset, 0, FLT_MAX);
    m_scrollOffset = ClampF(m_scrollOffset, 0, maxOffset);
}



//----------------------------------------------------------------------------------------------------------------------
void DevConsoleCommandHistory::SetNumLines(float numLines)
{
    m_numLines = numLines;
}



//----------------------------------------------------------------------------------------------------------------------
void DevConsoleCommandHistory::Clear()
{
    m_scrollOffset = 0.f;
    m_log.clear();
}
