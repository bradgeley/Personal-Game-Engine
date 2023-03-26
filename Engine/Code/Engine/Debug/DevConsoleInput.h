// Bradley Christensen - 2023
#pragma once
#include "Engine/Debug/DevConsoleLine.h"
#include <string>

#include "Engine/Events/EventDelegate.h"



class DevConsoleLog;
struct AABB2;



//----------------------------------------------------------------------------------------------------------------------
class DevConsoleInput
{
public:

    void SetOutputLog(DevConsoleLog* log);
    void Update(float deltaSeconds);
    
    void InputChar(uint8_t character);
    void SetLine(std::string const& line);

    void Delete();
    void Backspace();
    void Enter();
    void MoveCaret(int offset, bool wantsToSelect = false);
    bool IsSelecting() const;
    
    void RenderToBox(AABB2 const& box) const;
    void RenderSelection(AABB2 const& box) const;
    void RenderCaret(AABB2 const& box) const;

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
};

