﻿// Bradley Christensen - 2023
#include "DevConsoleInput.h"
#include "DevConsoleLog.h"
#include "Engine/DataStructures/NamedProperties.h"
#include "Engine/Input/InputUtils.h"
#include "Engine/Math/AABB2.h"
#include "Engine/Math/MathUtils.h"
#include "Engine/Renderer/Font.h"
#include "Engine/Renderer/Renderer.h"
#include "Engine/Renderer/VertexBuffer.h"
#include "Engine/Renderer/VertexUtils.h"
#include "Engine/Renderer/Window.h"



const std::string LINE_PREFIX = " > ";



//----------------------------------------------------------------------------------------------------------------------
DevConsoleInput::DevConsoleInput()
{
    m_vbo = new VertexBuffer();
}



//----------------------------------------------------------------------------------------------------------------------
DevConsoleInput::~DevConsoleInput()
{
    delete m_vbo;
    m_vbo = nullptr;
}



//----------------------------------------------------------------------------------------------------------------------
void DevConsoleInput::SetOutputLog(DevConsoleLog* log)
{
    m_outputLog = log;
}



//----------------------------------------------------------------------------------------------------------------------
void DevConsoleInput::Update(float deltaSeconds)
{
    m_caretAnimationFraction += deltaSeconds;
    if (m_caretAnimationFraction > 1.f)
    {
        m_caretAnimationFraction -= 1.f;
    }
}



//----------------------------------------------------------------------------------------------------------------------
void DevConsoleInput::InputChar(uint8_t character)
{
    if (IsSelecting())
    {
        DeleteSelection();
    }
    m_input.m_line.insert(m_input.m_line.begin() + m_caretIndex, (char) character);
    m_caretIndex++;
}



//----------------------------------------------------------------------------------------------------------------------
std::string DevConsoleInput::GetLine()
{
    return m_input.m_line;
}



//----------------------------------------------------------------------------------------------------------------------
void DevConsoleInput::SetLine(std::string const& line)
{
    m_caretIndex = 0;
    m_selectionStartIndex = -1;
    m_input.m_line = line;
}



//----------------------------------------------------------------------------------------------------------------------
void DevConsoleInput::Delete()
{
    if (IsSelecting())
    {
        DeleteSelection();
    }
    else if (!m_input.m_line.empty() && m_caretIndex != (int) m_input.m_line.size())
    {
        m_input.m_line.erase(m_input.m_line.begin() + m_caretIndex);
    }
}



//----------------------------------------------------------------------------------------------------------------------
void DevConsoleInput::Backspace()
{
    if (IsSelecting())
    {
        DeleteSelection();
    }
    else if (m_caretIndex > 0)
    {
        m_input.m_line.erase(m_input.m_line.begin() + m_caretIndex - 1);
        m_caretIndex--;
    }
}



//----------------------------------------------------------------------------------------------------------------------
void DevConsoleInput::Enter()
{
    if (m_input.m_line == "")
    {
        return;
    }
    
    m_outputLog->AddLine(m_input);
    
    NamedProperties args;
    args.Set("Command", m_input.m_line);
    m_commandEntered.Broadcast(args);
    
    m_input.m_line.clear();
    m_caretIndex = 0;
}



//----------------------------------------------------------------------------------------------------------------------
void DevConsoleInput::MoveCaret(int offset, bool wantsToSelect)
{
    if (!wantsToSelect && !IsSelecting())
    {
        // Normal case, not selecting and we don't already have something selected
        m_caretIndex += offset;
        m_caretIndex = ClampInt(m_caretIndex, 0, (int) m_input.m_line.size());
    }
    
    else if (!wantsToSelect && IsSelecting())
    {
        // We want to stop selecting, then move caret
        m_selectionStartIndex = -1;
        m_caretIndex += offset;
        m_caretIndex = ClampInt(m_caretIndex, 0, (int) m_input.m_line.size());
    }
    
    else if (wantsToSelect && IsSelecting())
    {
        // Continue the selection normally
        m_caretIndex += offset;
        m_caretIndex = ClampInt(m_caretIndex, 0, (int) m_input.m_line.size());
    }
    
    else if (wantsToSelect && !IsSelecting())
    {
        // We want to begin selecting
        int caretIndexBefore = m_caretIndex;
        m_caretIndex += offset;
        m_caretIndex = ClampInt(m_caretIndex, 0, (int) m_input.m_line.size());
        if (caretIndexBefore != m_caretIndex)
        {
            // We successfully moved the caret, so select
            m_selectionStartIndex = caretIndexBefore;
        }
    }

    if (m_selectionStartIndex == m_caretIndex)
    {
        // Not really selecting anymore
        m_selectionStartIndex = -1;
    }
}



//----------------------------------------------------------------------------------------------------------------------
bool DevConsoleInput::IsSelecting() const
{
    return m_selectionStartIndex != -1;
}



//----------------------------------------------------------------------------------------------------------------------
void DevConsoleInput::MoveCaretToEndOfLine()
{
    m_caretIndex = (int) m_input.m_line.size();
}



//----------------------------------------------------------------------------------------------------------------------
void DevConsoleInput::RenderToBox(AABB2 const& box) const
{
    RenderBackground(box);
    
    // Draw Input line selection and text
    float squeezeAmount = box.GetHeight() / 15.f;
    AABB2 textBox = box;
    textBox.Squeeze(squeezeAmount);
    
    RenderSelection(textBox);
    RenderText(textBox);
    RenderCaret(textBox);
}



//----------------------------------------------------------------------------------------------------------------------
void DevConsoleInput::RenderBackground(AABB2 const& box) const
{
    AddVertsForAABB2(m_vbo->GetMutableVerts(), box, Rgba8::DarkGray);

    g_renderer->BindShader(nullptr);
    g_renderer->BindTexture(nullptr);
    g_renderer->DrawVertexBuffer(m_vbo);
    m_vbo->ClearVerts();
}



//----------------------------------------------------------------------------------------------------------------------
void DevConsoleInput::RenderText(AABB2 const& box) const
{
    Font* font = g_renderer->GetDefaultFont(); // todo: let change fonts
    font->AddVertsForText2D(m_vbo->GetMutableVerts(), box.mins, box.GetHeight(), LINE_PREFIX + m_input.m_line, Rgba8::White);

    font->SetRendererState();
    g_renderer->DrawVertexBuffer(m_vbo);
    m_vbo->ClearVerts();
}



//----------------------------------------------------------------------------------------------------------------------
void DevConsoleInput::RenderSelection(AABB2 const& box) const
{
    if (!IsSelecting())
    {
        return;
    }
    
    Font* font = g_renderer->GetDefaultFont(); // todo: let change fonts
    
    float caretThickness = 1.f / (float) g_window->GetWidth();
    
    float textHeight = box.GetHeight();
    float selectionStartOffsetX = font->GetOffsetXOfCharIndex(LINE_PREFIX + m_input.m_line, m_selectionStartIndex + (int) LINE_PREFIX.size(), textHeight);
    float caretOffsetX = font->GetOffsetXOfCharIndex(LINE_PREFIX + m_input.m_line, m_caretIndex + (int) LINE_PREFIX.size(), textHeight);

    AABB2 selectionBox;
    selectionBox.mins = box.mins;
    selectionBox.mins.x += selectionStartOffsetX - caretThickness;
    selectionBox.maxs = box.mins;
    selectionBox.maxs.y = box.maxs.y;
    selectionBox.maxs.x += caretOffsetX + caretThickness;
    
    AddVertsForAABB2(m_vbo->GetMutableVerts(), selectionBox, Rgba8(0,171,240, 100));
    g_renderer->DrawVertexBuffer(m_vbo);
    m_vbo->ClearVerts();
}



//----------------------------------------------------------------------------------------------------------------------
void DevConsoleInput::RenderCaret(AABB2 const& box) const
{
    Font* font = g_renderer->GetDefaultFont(); // todo: let change fonts

    Rgba8 caretTint = Rgba8(255, 255, 255, (uint8_t) (m_caretAnimationFraction * 255.f));
    float textHeight = box.GetHeight();
    float caretThickness = 1.f / (float) g_window->GetWidth();
    
    AABB2 caretBox;
    float caretOffsetX = font->GetOffsetXOfCharIndex(LINE_PREFIX + m_input.m_line, m_caretIndex + (int) LINE_PREFIX.size(), textHeight);
    caretBox.mins = box.mins;
    caretBox.mins.x += caretOffsetX - caretThickness;
    caretBox.maxs = box.mins;
    caretBox.maxs.y = box.maxs.y;
    caretBox.maxs.x += caretOffsetX + caretThickness;
    
    AddVertsForAABB2(m_vbo->GetMutableVerts(), caretBox, caretTint);
    g_renderer->BindShader(nullptr);
    g_renderer->BindTexture(nullptr);
    g_renderer->DrawVertexBuffer(m_vbo);
    m_vbo->ClearVerts();
}



//----------------------------------------------------------------------------------------------------------------------
void DevConsoleInput::DeleteSelection()
{
    int lowerIndex = Min(m_selectionStartIndex, m_caretIndex);
    int upperIndex = Max(m_selectionStartIndex, m_caretIndex);
    m_input.m_line.erase(m_input.m_line.begin() + lowerIndex, m_input.m_line.begin() + upperIndex);
    m_selectionStartIndex = -1;
    m_caretIndex = lowerIndex;
}