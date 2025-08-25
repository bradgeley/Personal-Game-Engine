﻿// Bradley Christensen - 2023
#include "DevConsoleCommandHistory.h"

#include "Engine/Core/FileUtils.h"
#include "Engine/Core/StringUtils.h"
#include "Engine/Math/AABB2.h"
#include "Engine/Math/MathUtils.h"
#include "Engine/Renderer/Font.h"
#include "Engine/Renderer/RendererInterface.h"
#include "Engine/Renderer/VertexBuffer.h"
#include "Engine/Renderer/VertexUtils.h"



//----------------------------------------------------------------------------------------------------------------------
DevConsoleCommandHistory::DevConsoleCommandHistory()
{
    m_textVerts = g_rendererInterface->MakeVertexBuffer();
    m_untexturedVerts = g_rendererInterface->MakeVertexBuffer();
}



//----------------------------------------------------------------------------------------------------------------------
DevConsoleCommandHistory::~DevConsoleCommandHistory()
{
    m_untexturedVerts->ReleaseResources();
    delete m_untexturedVerts;
    m_untexturedVerts = nullptr;

    m_textVerts->ReleaseResources();
    delete m_textVerts;
    m_textVerts = nullptr;
}



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
    Font* font = g_rendererInterface->GetDefaultFont(); // todo: let change fonts

    // Draw Background
    m_untexturedVerts->ClearVerts();
    AddVertsForAABB2(m_untexturedVerts->GetMutableVerts(), box, Rgba8(0, 0, 0, 200));
    AddVertsForWireBox2D(m_untexturedVerts->GetMutableVerts(), box, 0.0025f, Rgba8(255, 255, 255, 200));

    m_textVerts->ClearVerts();

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
            AddVertsForAABB2(m_untexturedVerts->GetMutableVerts(), textBox, Rgba8::Cerulean);
        }
        
        font->AddVertsForText2D(m_textVerts->GetMutableVerts(), textBox.mins, textBox.GetHeight(), "> " + m_log[i], Rgba8::White);
        
        linesRendered += 1.f;
    }

    g_rendererInterface->BindShader(nullptr);
    g_rendererInterface->BindTexture(nullptr);
    g_rendererInterface->DrawVertexBuffer(m_untexturedVerts);

    font->SetRendererState();
    g_rendererInterface->DrawVertexBuffer(m_textVerts);
}



//----------------------------------------------------------------------------------------------------------------------
void DevConsoleCommandHistory::ArrowUp()
{
    // Decrement on arrow up so the index is reversed when rendering
    m_selectedLineIndex = (int8_t) MathUtils::DecrementIntInRange(m_selectedLineIndex, -1, (int) m_log.size() - 1, true);
}



//----------------------------------------------------------------------------------------------------------------------
void DevConsoleCommandHistory::ArrowDown()
{
    // Decrement on arrow up so the index is reversed when rendering
    m_selectedLineIndex = (int8_t) MathUtils::IncrementIntInRange(m_selectedLineIndex, -1, (int) m_log.size() - 1, true);
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
    for (std::string const& line : m_log)
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
    for (size_t i = 0; i < m_log.size(); ++i)
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
