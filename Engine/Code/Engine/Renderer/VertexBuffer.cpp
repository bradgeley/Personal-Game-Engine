// Bradley Christensen - 2022-2023
#include "VertexBuffer.h"
#include "RendererInterface.h"
#include "Engine/Core/ErrorUtils.h"



//----------------------------------------------------------------------------------------------------------------------
VertexBuffer::~VertexBuffer()
{
    ReleaseResources();
}



//----------------------------------------------------------------------------------------------------------------------
void VertexBuffer::ReleaseResources()
{
    // Empty
}



//----------------------------------------------------------------------------------------------------------------------
std::vector<Vertex_PCU> const& VertexBuffer::GetVerts() const
{
    return m_verts;
}



//----------------------------------------------------------------------------------------------------------------------
std::vector<Vertex_PCU>& VertexBuffer::GetMutableVerts(bool setDirty /*= true*/)
{
    if (setDirty)
    {
        // By default, assume that the vertex array has been changed after this function call
        SetDirty();
    }
    return m_verts;
}



//----------------------------------------------------------------------------------------------------------------------
void VertexBuffer::AddVerts(std::vector<Vertex_PCU> const& verts)
{
    SetDirty();
    m_verts.insert(m_verts.end(), verts.begin(), verts.end());
}



//----------------------------------------------------------------------------------------------------------------------
void VertexBuffer::ReserveAdditional(int numExpectedAdditionalVerts)
{
    size_t numVerts = m_verts.size();
    m_verts.reserve(numVerts + (size_t) numExpectedAdditionalVerts);
}



//----------------------------------------------------------------------------------------------------------------------
void VertexBuffer::ClearVerts()
{
    SetDirty();
    m_verts.clear();
}



//----------------------------------------------------------------------------------------------------------------------
int VertexBuffer::GetStride() const
{
    return sizeof(Vertex_PCU); // todo: templatize for different vert layouts?
}



//----------------------------------------------------------------------------------------------------------------------
int VertexBuffer::GetNumVerts() const
{
    return (int) m_verts.size();  
}



//----------------------------------------------------------------------------------------------------------------------
bool VertexBuffer::IsDirty() const
{
    return m_isDirty;
}



//----------------------------------------------------------------------------------------------------------------------
void VertexBuffer::SetDirty()
{
    m_isDirty = true;
}



//----------------------------------------------------------------------------------------------------------------------
bool VertexBuffer::IsEmpty() const
{
    return m_verts.empty();
}
