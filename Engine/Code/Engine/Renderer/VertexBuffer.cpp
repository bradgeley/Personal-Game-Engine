// Bradley Christensen - 2022-2023
#include "VertexBuffer.h"
#include "RendererInternal.h"
#include "Renderer.h"
#include "Engine/Core/ErrorUtils.h"



//----------------------------------------------------------------------------------------------------------------------
VertexBuffer::~VertexBuffer()
{
    ReleaseResources();
}



//----------------------------------------------------------------------------------------------------------------------
void VertexBuffer::Initialize(int numExpectedVerts)
{
    DX_SAFE_RELEASE(m_handle)

    // Release for reinitialization if already initialized
    ASSERT_OR_DIE(numExpectedVerts > 0, "Cannot initialize vbo with 0 or fewer verts");

    ID3D11Device* device = g_renderer->GetDevice();
    ASSERT_OR_DIE(device, "Renderer device is null, has Renderer called Startup yet?");

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

    m_verts.reserve(numExpectedVerts);
}



//----------------------------------------------------------------------------------------------------------------------
void VertexBuffer::ReserveAdditional(int numExpectedAdditionalVerts)
{
    int numVerts = m_verts.size();
    m_verts.reserve(numVerts + numExpectedAdditionalVerts);
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
