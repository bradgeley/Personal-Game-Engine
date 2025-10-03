// Bradley Christensen - 2022-2025
#pragma once
#include "Engine/Debug/DevConsoleLine.h"
#include "Engine/Events/EventDelegate.h"
#include "Engine/Renderer/RendererUtils.h"
#include <string>



class DevConsoleLog;
struct AABB2;



//----------------------------------------------------------------------------------------------------------------------
class DevConsoleInput
{
public:

    void Startup();
    void Shutdown();

    void SetOutputLog(DevConsoleLog* log);
    void Update(float deltaSeconds);
    
    void InputChar(uint8_t character);
    std::string GetLine();
    void SetLine(std::string const& line);

    void Delete();
    void DeleteAll();
    void Backspace();
    void Enter();
    void MoveCaret(int offset, bool wantsToSelect = false);
    bool IsSelecting() const;
    void MoveCaretToEndOfLine();
    
    void RenderToBox(AABB2 const& box) const;
    void RenderBackground(AABB2 const& box) const;
    void RenderText(AABB2 const& box) const;
    void RenderSelection(AABB2 const& box) const;
    void RenderCaret(AABB2 const& box) const;

	void RenderInputGuessesToBox(AABB2 const& box, int maxLines = 10) const;

	void CopyToClipboard() const;
	void PasteFromClipboard();

private:

    void DeleteSelection();

public:

    EventDelegate m_commandEntered;
    
protected:

    DevConsoleLog* m_outputLog = nullptr;
    int m_caretIndex = 0;
    int m_selectionStartIndex = -1; // -1 means not selecting anything
    float m_caretAnimationFraction = 0.f;
    DevConsoleLine m_input;
    VertexBufferID m_vbo = RendererUtils::InvalidID;
};

