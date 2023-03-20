// Bradley Christensen - 2023
#include "DevConsoleInput.h"
#include "DevConsoleLog.h"
#include "Engine/Input/InputUtils.h"
#include "Engine/Math/AABB2.h"
#include "Engine/Renderer/Font.h"
#include "Engine/Renderer/Renderer.h"
#include "Engine/Renderer/VertexBuffer.h"
#include "Engine/Renderer/VertexUtils.h"
#include "Engine/Renderer/Window.h"



const std::string LINE_PREFIX = " > ";



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
    m_input.insert(m_input.begin() + m_caretIndex, (char) character);
    m_caretIndex++;
}



//----------------------------------------------------------------------------------------------------------------------
void DevConsoleInput::RenderToBox(AABB2 const& box) const
{
    Font* font = g_renderer->GetDefaultFont(); // todo: let change fonts

    // Draw Background
    VertexBuffer backgroundBuffer;
    auto& bgVerts = backgroundBuffer.GetMutableVerts();
    AddVertsForAABB2(bgVerts, box, Rgba8::DarkGray);

    g_renderer->BindShader(nullptr);
    g_renderer->BindTexture(nullptr);
    g_renderer->DrawVertexBuffer(&backgroundBuffer);
    //

    
    // Draw Input line text
    float squeeze = box.GetHeight() / 15.f;
    AABB2 textBox = box;
    textBox.Squeeze(squeeze);
    
    VertexBuffer textBuffer;
    auto& verts = textBuffer.GetMutableVerts();
    font->AddVertsForText2D(verts, textBox.mins, textBox.GetHeight(), LINE_PREFIX + m_input, Rgba8::White);
    
    font->SetRendererState();
    g_renderer->DrawVertexBuffer(&textBuffer);
    //

    // Draw Caret
    g_renderer->BindShader(nullptr);
    g_renderer->BindTexture(nullptr);
    RenderCaret(textBox);
    //
}



//----------------------------------------------------------------------------------------------------------------------
void DevConsoleInput::Delete()
{
    if (!m_input.empty() && m_caretIndex != (int) m_input.size() - 1)
    {
        m_input.erase(m_input.begin() + m_caretIndex);
    }
}



//----------------------------------------------------------------------------------------------------------------------
void DevConsoleInput::Backspace()
{
    if (m_caretIndex > 0)
    {
        m_input.erase(m_input.begin() + m_caretIndex - 1);
        m_caretIndex--;
    }
}



//----------------------------------------------------------------------------------------------------------------------
void DevConsoleInput::Enter()
{
    m_outputLog->AddLine(m_input);
    m_input.clear();
    m_caretIndex = 0;
}



//----------------------------------------------------------------------------------------------------------------------
void DevConsoleInput::RenderCaret(AABB2 const& box) const
{
    Font* font = g_renderer->GetDefaultFont(); // todo: let change fonts

    Rgba8 caretTint = Rgba8(255, 255, 255, (uint8_t) (m_caretAnimationFraction * 255.f));
    float textHeight = box.GetHeight();
    float caretThickness = 1.f / (float) g_window->GetWidth();
    
    AABB2 caretBox;
    float caretOffsetX = font->GetOffsetXOfCharIndex(LINE_PREFIX + m_input, m_caretIndex + (int) LINE_PREFIX.size(), textHeight);
    caretBox.mins = box.mins;
    caretBox.mins.x += caretOffsetX - caretThickness;
    caretBox.maxs = box.mins;
    caretBox.maxs.y = box.maxs.y;
    caretBox.maxs.x += caretOffsetX + caretThickness;
    
    VertexBuffer buffer;
    auto& verts = buffer.GetMutableVerts();
    AddVertsForAABB2(verts, caretBox, caretTint);
    g_renderer->DrawVertexBuffer(&buffer);
}
