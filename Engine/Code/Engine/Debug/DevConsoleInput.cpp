// Bradley Christensen - 2022-2025
#include "DevConsoleInput.h"
#include "DevConsoleLog.h"
#include "Engine/Math/AABB2.h"
#include "Engine/Math/MathUtils.h"
#include "Engine/Renderer/Font.h"
#include "Engine/Renderer/Renderer.h"
#include "Engine/Renderer/VertexBuffer.h"
#include "Engine/Renderer/VertexUtils.h"
#include "Engine/Renderer/Window.h"
#include "Engine/Debug/DevConsole.h"
#include "Engine/DataStructures/NamedProperties.h"



const std::string LINE_PREFIX = " > ";



//----------------------------------------------------------------------------------------------------------------------
DevConsoleInput::DevConsoleInput()
{
    m_vbo = g_renderer->MakeVertexBuffer();
}



//----------------------------------------------------------------------------------------------------------------------
DevConsoleInput::~DevConsoleInput()
{
    g_renderer->ReleaseVertexBuffer(m_vbo);
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
    while (m_caretAnimationFraction > 1.f)
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
        m_caretIndex = MathUtils::ClampInt(m_caretIndex, 0, (int) m_input.m_line.size());
    }
    
    else if (!wantsToSelect && IsSelecting())
    {
        // We want to stop selecting, then move caret
        m_selectionStartIndex = -1;
        m_caretIndex += offset;
        m_caretIndex = MathUtils::ClampInt(m_caretIndex, 0, (int) m_input.m_line.size());
    }
    
    else if (wantsToSelect && IsSelecting())
    {
        // Continue the selection normally
        m_caretIndex += offset;
        m_caretIndex = MathUtils::ClampInt(m_caretIndex, 0, (int) m_input.m_line.size());
    }
    
    else if (wantsToSelect && !IsSelecting())
    {
        // We want to begin selecting
        int caretIndexBefore = m_caretIndex;
        m_caretIndex += offset;
        m_caretIndex = MathUtils::ClampInt(m_caretIndex, 0, (int) m_input.m_line.size());
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
    VertexBuffer& vbo = *g_renderer->GetVertexBuffer(m_vbo);
    vbo.ClearVerts();
    AddVertsForAABB2(vbo, box, Rgba8::DarkGray);

    g_renderer->BindShader(nullptr);
    g_renderer->BindTexture(nullptr);
    g_renderer->DrawVertexBuffer(vbo);
}



//----------------------------------------------------------------------------------------------------------------------
void DevConsoleInput::RenderText(AABB2 const& box) const
{
    Font* font = g_renderer->GetDefaultFont(); // todo: let change fonts

    std::string guess = g_devConsole->GuessCommandInput(m_input.m_line);

    VertexBuffer& vbo = *g_renderer->GetVertexBuffer(m_vbo);
    vbo.ClearVerts();

    float alpha = MathUtils::RangeMapClamped(m_caretAnimationFraction, 0.f, 1.f, 75.f, 100.f);
    font->AddVertsForText2D(vbo, box.mins, box.GetHeight(), LINE_PREFIX + guess, Rgba8(155, 155, 155, static_cast<uint8_t>(alpha)));
    font->AddVertsForText2D(vbo, box.mins, box.GetHeight(), LINE_PREFIX + m_input.m_line, Rgba8::White);

    font->SetRendererState();
    g_renderer->DrawVertexBuffer(vbo);
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

    VertexBuffer& vbo = *g_renderer->GetVertexBuffer(m_vbo);
    vbo.ClearVerts();
    
    AddVertsForAABB2(vbo, selectionBox, Rgba8(0,171,240, 100));
    g_renderer->DrawVertexBuffer(vbo);
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

    VertexBuffer& vbo = *g_renderer->GetVertexBuffer(m_vbo);
    vbo.ClearVerts();
    
    AddVertsForAABB2(vbo, caretBox, caretTint);
    g_renderer->BindShader(nullptr);
    g_renderer->BindTexture(nullptr);
    g_renderer->DrawVertexBuffer(vbo);
}



//----------------------------------------------------------------------------------------------------------------------
void DevConsoleInput::DeleteSelection()
{
    int lowerIndex = MathUtils::Min(m_selectionStartIndex, m_caretIndex);
    int upperIndex = MathUtils::Max(m_selectionStartIndex, m_caretIndex);
    m_input.m_line.erase(m_input.m_line.begin() + lowerIndex, m_input.m_line.begin() + upperIndex);
    m_selectionStartIndex = -1;
    m_caretIndex = lowerIndex;
}