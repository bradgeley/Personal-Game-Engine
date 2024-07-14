// Bradley Christensen - 2022-2023
#include "Engine/Renderer/VertexBuffer.h"
#include "Renderer.h"
#include "Engine/Core/ErrorUtils.h"
#include "Engine/Renderer/RendererInternal.h"



//----------------------------------------------------------------------------------------------------------------------
VertexBuffer::~VertexBuffer()
{
    ReleaseResources();
}



//----------------------------------------------------------------------------------------------------------------------
void VertexBuffer::Initialize(int numExpectedVerts)
{
    // Release for reinitialization if already initialized
    DX_SAFE_RELEASE(m_handle)
    
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
    
    ASSERT_OR_DIE(SUCCEEDED(result), "Failed to create gpu vertex buffer")

    m_gpuBufferByteWidth = (size_t) byteWidth;
}



//----------------------------------------------------------------------------------------------------------------------
void VertexBuffer::ReleaseResources()
{
    DX_SAFE_RELEASE(m_handle)
    m_gpuBufferByteWidth = 0;
}



//----------------------------------------------------------------------------------------------------------------------
std::vector<Vertex_PCU> const& VertexBuffer::GetVerts() const
{
    return m_verts;
}



//----------------------------------------------------------------------------------------------------------------------
std::vector<Vertex_PCU>& VertexBuffer::GetMutableVerts()
{
    m_isDirty = true; // for now just dirty the verts any time they are read as a mutable array, (assume changes were made)
    return m_verts;
}



//----------------------------------------------------------------------------------------------------------------------
void VertexBuffer::AddVerts(std::vector<Vertex_PCU> const& verts)
{
    m_isDirty = true;
    m_verts.insert(m_verts.end(), verts.begin(), verts.end());
}



//----------------------------------------------------------------------------------------------------------------------
void VertexBuffer::ClearVerts()
{
    m_isDirty = true;
    m_verts.clear();
}



//----------------------------------------------------------------------------------------------------------------------
int VertexBuffer::GetStride() const
{
    return sizeof(Vertex_PCU);
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
void VertexBuffer::UpdateGPUBuffer()
{
    m_isDirty = false;
    
    if (m_verts.empty())
    {
        ReleaseResources();
        return;
    }

    size_t bytesNeeded = m_verts.size() * GetStride();
    if (bytesNeeded > m_gpuBufferByteWidth)
    {
        Initialize((int) m_verts.size());
    }

    auto deviceContext = g_renderer->GetContext();
    
    D3D11_MAPPED_SUBRESOURCE mapping;
    ID3D11Buffer* gpuBuffer = m_handle;

    HRESULT result = deviceContext->Map(
        gpuBuffer,
        0,
        D3D11_MAP_WRITE_DISCARD,
        0,
        &mapping
   );
    
	ASSERT_OR_DIE(SUCCEEDED(result), "Failed to map vertex buffer to gpu buffer");
    
	memcpy(mapping.pData, m_verts.data(), bytesNeeded);

    deviceContext->Unmap(gpuBuffer, 0);
}
