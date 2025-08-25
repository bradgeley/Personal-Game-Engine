// Bradley Christensen - 2023
#pragma once
#include "DevConsoleLine.h"
#include <vector>



struct AABB2;
class VertexBuffer;



//----------------------------------------------------------------------------------------------------------------------
class DevConsoleLog
{
public:

    DevConsoleLog();
    ~DevConsoleLog();

    void AddLine(DevConsoleLine const& line);
    void RenderToBox(AABB2 const& box) const;
    void Scroll(int scrollAmount);
    void SetNumLines(float numLines);
    void Clear();

protected:

    float m_numLines = 20.5f;
    float m_scrollOffset = 0.f;
    std::vector<DevConsoleLine> m_log;
    VertexBuffer* m_vbo = nullptr;
};
