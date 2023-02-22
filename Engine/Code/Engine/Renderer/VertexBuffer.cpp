// Bradley Christensen - 2022
#include "Engine/Renderer/VertexBuffer.h"

#include "Renderer.h"
#include "Engine/Core/ErrorUtils.h"
#include "Engine/Renderer/RendererInternal.h"



void VertexBuffer::Initialize(int numExpectedVerts)
{
    ID3D11Device* device = g_renderer->GetDevice();
    uint32_t byteWidth = numExpectedVerts * GetStride();
    
    D3D11_BUFFER_DESC desc;
    desc.ByteWidth = byteWidth;
    desc.Usage = D3D11_USAGE_DYNAMIC;
    desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
    desc.MiscFlags = 0;
    desc.StructureByteStride = 0;

    HRESULT result = device->CreateBuffer(&desc, nullptr, &m_handle);
    
    ASSERT_OR_DIE( SUCCEEDED( result ), "Failed to create vertex buffer")
}



void VertexBuffer::AddVerts(std::vector<Vertex_PCU> const& verts)
{
    m_verts.resize(m_verts.size() + verts.size());
    Vertex_PCU* memLocToCopy = m_verts.data() + m_verts.size();
    memcpy(memLocToCopy, verts.data(), verts.size());
}



int VertexBuffer::GetStride() const
{
    return sizeof(Vertex_PCU);
}



int VertexBuffer::GetNumVerts() const
{
    return (int) m_verts.size();
}
