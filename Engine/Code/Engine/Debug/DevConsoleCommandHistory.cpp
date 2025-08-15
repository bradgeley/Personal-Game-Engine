// Bradley Christensen - 2023
#include "DevConsoleCommandHistory.h"

#include "Engine/Core/FileUtils.h"
#include "Engine/Core/StringUtils.h"
#include "Engine/Math/AABB2.h"
#include "Engine/Math/MathUtils.h"
#include "Engine/Renderer/Font.h"
#include "Engine/Renderer/Renderer.h"
#include "Engine/Renderer/VertexBuffer.h"
#include "Engine/Renderer/VertexUtils.h"



//----------------------------------------------------------------------------------------------------------------------
void DevConsoleCommandHistory::AddCommand(std::string const& command)
{
    if (TryMoveCommandToBottomOfList(command))
    {
        return;
    }
    
    if ((int8_t) m_log.size() == m_maxHistorySize)
    {
        m_log.erase(m_log.begin());
    }
    m_log.emplace_back(command);
}



//----------------------------------------------------------------------------------------------------------------------
std::string DevConsoleCommandHistory::GetSelectedCommand() const
{
    if (m_selectedLineIndex == (int8_t) -1)
    {
        return "";
    }
    return m_log[m_selectedLineIndex];
}



//----------------------------------------------------------------------------------------------------------------------
void DevConsoleCommandHistory::Close()
{
    m_selectedLineIndex = -1;
}



//----------------------------------------------------------------------------------------------------------------------
bool DevConsoleCommandHistory::IsActive() const
{
    return m_selectedLineIndex != (int8_t) -1;
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

    float lineThickness = box.GetHeight() / (float) m_maxHistorySize;

    float linesRendered = 0.f;
    for (int i = (int) m_log.size() - 1; i >= 0; --i)
    {
        float yOffsetBot = linesRendered * lineThickness;
        float yOffsetTop = (linesRendered + 1.f) * lineThickness;
        AABB2 textBox = AABB2(box.mins.x, box.mins.y + yOffsetBot, box.maxs.x, box.mins.y + yOffsetTop);
        float squeeze = textBox.GetHeight() / 15.f;
        textBox.Squeeze(squeeze);
        
        if (m_selectedLineIndex == i)
        {
            AddVertsForAABB2(bgVerts, textBox, Rgba8::Cerulean);
            g_renderer->BindShader(nullptr);
            g_renderer->BindTexture(nullptr);
            g_renderer->DrawVertexBuffer(&backgroundBuffer);
        }
        
        font->AddVertsForText2D(verts, textBox.mins, textBox.GetHeight(), "> " + m_log[i], Rgba8::White);
        
        linesRendered += 1.f;
    }

    font->SetRendererState();
    g_renderer->DrawVertexBuffer(&buffer);
}



//----------------------------------------------------------------------------------------------------------------------
void DevConsoleCommandHistory::ArrowUp()
{
    // Decrement on arrow up so the index is reversed when rendering
    m_selectedLineIndex = (int8_t) DecrementIntInRange(m_selectedLineIndex, -1, (int) m_log.size() - 1, true);
}



//----------------------------------------------------------------------------------------------------------------------
void DevConsoleCommandHistory::ArrowDown()
{
    // Decrement on arrow up so the index is reversed when rendering
    m_selectedLineIndex = (int8_t) IncrementIntInRange(m_selectedLineIndex, -1, (int) m_log.size() - 1, true);
}



//----------------------------------------------------------------------------------------------------------------------
void DevConsoleCommandHistory::Clear()
{
    m_log.clear();
    m_selectedLineIndex = -1;
}



//----------------------------------------------------------------------------------------------------------------------
bool DevConsoleCommandHistory::LoadFrom(std::string const& filepath)
{
    std::string fileContents;
    if ((bool) FileReadToString(filepath, fileContents))
    {
        Strings strings = StringUtils::SplitStringOnDelimeter(fileContents, '\n');
        for (std::string const& splitString : strings)
        {
            AddCommand(splitString);
        }
        return true;
    }

    return false;
}



//----------------------------------------------------------------------------------------------------------------------
bool DevConsoleCommandHistory::SaveTo(std::string const& filepath) const
{
    std::string historyAsOneString;
    for (auto& line : m_log)
    {
        historyAsOneString += line;
        historyAsOneString += '\n';
    }

    if (!historyAsOneString.empty())
    {
        return (bool) FileWriteFromString(filepath, historyAsOneString);
    }

    // No history to write = succeeded
    return true;
}



//----------------------------------------------------------------------------------------------------------------------
bool DevConsoleCommandHistory::TryMoveCommandToBottomOfList(std::string const& command)
{
    for (int i = 0; i < (int) m_log.size(); ++i)
    {
        if (m_log[i] == command)
        {
            m_log.erase(m_log.begin() + i);
            m_log.emplace_back(command);
            return true;
        }
    }
    return false;
}
