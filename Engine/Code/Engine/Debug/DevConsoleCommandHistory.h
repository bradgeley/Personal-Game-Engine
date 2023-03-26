// Bradley Christensen - 2023
#pragma once
#include <string>
#include <vector>



struct AABB2;



//----------------------------------------------------------------------------------------------------------------------
class DevConsoleCommandHistory
{
public:

    void OnCommandEntered(std::string const& command);
    std::string GetSelectedCommand() const;

    bool IsActive() const;
    void RenderToBox(AABB2 const& box) const;
    
    void ArrowUp();
    void ArrowDown();
    void Scroll(int scrollAmount);
    void SetNumLines(float numLines);
    void Clear();

protected:

    float m_numLines = 10.5f;
    float m_scrollOffset = 0.f;
    int m_selectedLineIndex = -1;
    std::vector<std::string> m_log;
};
