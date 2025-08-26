// Bradley Christensen - 2022-2025
#pragma once
#include "Engine/Renderer/RendererUtils.h"
#include <string>
#include <vector>



struct AABB2;



//----------------------------------------------------------------------------------------------------------------------
class DevConsoleCommandHistory
{
public:

    DevConsoleCommandHistory();
    ~DevConsoleCommandHistory();

    void AddCommand(std::string const& command);
    std::string GetSelectedCommand() const;

    void Close();
    bool IsActive() const;
    void RenderToBox(AABB2 const& box) const;
    
    void ArrowUp();
    void ArrowDown();
    void Clear();

    bool LoadFrom(std::string const& filepath);
    bool SaveTo(std::string const& filepath) const;

private:

    bool TryMoveCommandToBottomOfList(std::string const& command);

protected:

    std::vector<std::string> m_log;
    int8_t m_maxHistorySize = 10;
    int8_t m_selectedLineIndex = -1;
    VertexBufferID m_untexturedVerts = RendererUtils::InvalidID;
    VertexBufferID m_textVerts = RendererUtils::InvalidID;
};
