// Bradley Christensen - 2022-2023
#include "D3D11VertexBuffer.h"
#include "D3D11Internal.h"
#include "D3D11Renderer.h"
#include "Engine/Core/ErrorUtils.h"



//----------------------------------------------------------------------------------------------------------------------
void D3D11VertexBuffer::Initialize(int numExpectedVerts)
{
    DX_SAFE_RELEASE(m_handle)

    // Release for reinitialization if already initialized
    ASSERT_OR_DIE(numExpectedVerts > 0, "Cannot initialize vbo with 0 or fewer verts");

    ID3D11Device* device = D3D11Renderer::Get()->GetDevice();
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
void D3D11VertexBuffer::UpdateGPUBuffer()
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

    ID3D11DeviceContext* deviceContext = D3D11Renderer::Get()->GetDeviceContext();
    
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



//----------------------------------------------------------------------------------------------------------------------
void D3D11VertexBuffer::ReleaseResources()
{
    DX_SAFE_RELEASE(m_handle)
        m_gpuBufferByteWidth = 0;
}