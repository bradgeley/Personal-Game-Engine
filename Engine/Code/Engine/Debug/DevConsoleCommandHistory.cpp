// Bradley Christensen - 2022-2026
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
void DevConsoleCommandHistory::Startup()
{
    m_textVerts = g_renderer->MakeVertexBuffer<Vertex_PCU>();
    m_untexturedVerts = g_renderer->MakeVertexBuffer<Vertex_PCU>();
}



//----------------------------------------------------------------------------------------------------------------------
void DevConsoleCommandHistory::Shutdown()
{
    g_renderer->ReleaseVertexBuffer(m_textVerts);
    g_renderer->ReleaseVertexBuffer(m_untexturedVerts);
}



//----------------------------------------------------------------------------------------------------------------------
void DevConsoleCommandHistory::AddCommand(std::string const& command)
{
    if (TryMoveCommandToBottomOfList(command))
    {
        return;
    }
    
    if ((int8_t) m_commandHistory.size() == m_maxHistorySize)
    {
        m_commandHistory.erase(m_commandHistory.begin());
    }
    m_commandHistory.emplace_back(command);
}



//----------------------------------------------------------------------------------------------------------------------
std::string DevConsoleCommandHistory::GetSelectedCommand() const
{
    if (m_selectedLineIndex == (int8_t) -1)
    {
        return "";
    }
    return m_commandHistory[m_selectedLineIndex];
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

    VertexBuffer& untexturedVerts = *g_renderer->GetVertexBuffer(m_untexturedVerts);
    VertexBuffer& textVerts = *g_renderer->GetVertexBuffer(m_textVerts);

    // Draw Background
    untexturedVerts.ClearVerts();
    VertexUtils::AddVertsForAABB2(untexturedVerts, box, Rgba8(0, 0, 0, 200));
    VertexUtils::AddVertsForWireBox2D(untexturedVerts, box, 0.0025f, Rgba8(255, 255, 255, 200));

    textVerts.ClearVerts();

    float lineThickness = box.GetHeight() / (float) m_maxHistorySize;

    float linesRendered = 0.f;
    for (int i = (int) m_commandHistory.size() - 1; i >= 0; --i)
    {
        float yOffsetBot = linesRendered * lineThickness;
        float yOffsetTop = (linesRendered + 1.f) * lineThickness;
        AABB2 textBox = AABB2(box.mins.x, box.mins.y + yOffsetBot, box.maxs.x, box.mins.y + yOffsetTop);
        float squeeze = textBox.GetHeight() / 15.f;
        textBox.Squeeze(squeeze);
        
        if (m_selectedLineIndex == i)
        {
            VertexUtils::AddVertsForAABB2(untexturedVerts, textBox, Rgba8::Cerulean);
        }
        
        font->AddVertsForText2D(textVerts, textBox.mins, textBox.GetHeight(), "> " + m_commandHistory[i], Rgba8::White);
        
        linesRendered += 1.f;
    }

    g_renderer->BindShader(nullptr);
    g_renderer->BindTexture(nullptr);
    g_renderer->DrawVertexBuffer(untexturedVerts);

    font->SetRendererState();
    g_renderer->DrawVertexBuffer(textVerts);
}



//----------------------------------------------------------------------------------------------------------------------
void DevConsoleCommandHistory::ArrowUp()
{
    // Decrement on arrow up so the index is reversed when rendering
    m_selectedLineIndex = (int8_t) MathUtils::DecrementIntInRange(m_selectedLineIndex, -1, (int) m_commandHistory.size() - 1, true);
}



//----------------------------------------------------------------------------------------------------------------------
void DevConsoleCommandHistory::ArrowDown()
{
    // Decrement on arrow up so the index is reversed when rendering
    m_selectedLineIndex = (int8_t) MathUtils::IncrementIntInRange(m_selectedLineIndex, -1, (int) m_commandHistory.size() - 1, true);
}



//----------------------------------------------------------------------------------------------------------------------
void DevConsoleCommandHistory::Clear()
{
    m_commandHistory.clear();
    m_selectedLineIndex = -1;
}



//----------------------------------------------------------------------------------------------------------------------
bool DevConsoleCommandHistory::LoadFrom(std::string const& filepath)
{
    std::string fileContents;
    int bytesRead = FileUtils::FileReadToString(filepath, fileContents);
    if (bytesRead > 0)
    {
        Strings strings = StringUtils::SplitStringOnDelimiter(fileContents, '\n');
        for (std::string const& splitString : strings)
        {
            if (!splitString.empty()) 
            {
                AddCommand(splitString);
            }
        }
        return true;
    }

    return false;
}



//----------------------------------------------------------------------------------------------------------------------
bool DevConsoleCommandHistory::SaveTo(std::string const& filepath) const
{
    std::string historyAsOneString;
    for (std::string const& line : m_commandHistory)
    {
        if (line.empty()) 
        {
			continue;
        }

        historyAsOneString += line;
        historyAsOneString += '\n';
    }

    if (!historyAsOneString.empty())
    {
		int bytesWritten = FileUtils::FileWriteFromString(filepath, historyAsOneString);
        return bytesWritten > 0;
    }

    // No history to write = succeeded
    return true;
}



//----------------------------------------------------------------------------------------------------------------------
bool DevConsoleCommandHistory::TryMoveCommandToBottomOfList(std::string const& command)
{
    for (size_t i = 0; i < m_commandHistory.size(); ++i)
    {
        if (m_commandHistory[i] == command)
        {
            m_commandHistory.erase(m_commandHistory.begin() + i);
            m_commandHistory.emplace_back(command);
            return true;
        }
    }
    return false;
}
