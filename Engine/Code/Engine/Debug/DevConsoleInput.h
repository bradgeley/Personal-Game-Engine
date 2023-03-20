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
    void RenderToBox(AABB2 const& box) const;

    void Delete();
    void Backspace();
    void Enter();
    void RenderCaret(AABB2 const& box) const;

public:

    EventDelegate m_commandEntered;
    
protected:

    DevConsoleLog* m_outputLog = nullptr;
    int m_caretIndex = 0;
    float m_caretAnimationFraction = 0.f;
    DevConsoleLine m_input;
};
