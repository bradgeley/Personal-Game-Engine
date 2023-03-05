// Bradley Christensen - 2022-2023
#include "Engine/Renderer/ConstantBuffer.h"
#include "Renderer.h"
#include "Engine/Core/ErrorUtils.h"
#include "Engine/Renderer/RendererInternal.h"



//----------------------------------------------------------------------------------------------------------------------
void ConstantBuffer::Update(void const* data, size_t byteWidth)
{
    if (!m_handle || byteWidth > m_gpuBufferByteWidth)
    {
        Initialize(byteWidth);
    }
    
    // GPU
    D3D11_MAPPED_SUBRESOURCE mapping;
    HRESULT result = g_renderer->GetContext()->Map(
        m_handle,
        0,
        D3D11_MAP_WRITE_DISCARD,
        0,
        &mapping
   );
    
    ASSERT_OR_DIE(SUCCEEDED(result), "Failed to map data for constant buffer")

    memcpy(mapping.pData, data, byteWidth);

    g_renderer->GetContext()->Unmap(m_handle, 0);
}



//----------------------------------------------------------------------------------------------------------------------
void ConstantBuffer::Initialize(size_t byteWidth)
{
    if (m_handle)
    {
        DX_SAFE_RELEASE(m_handle)
    }
    
    D3D11_BUFFER_DESC desc;
    desc.ByteWidth = (UINT) byteWidth;
    desc.Usage = D3D11_USAGE_DYNAMIC;
    desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
    desc.MiscFlags = 0;
    desc.StructureByteStride = 0;
    
    HRESULT result = g_renderer->GetDevice()->CreateBuffer(&desc, nullptr, &m_handle);
    ASSERT_OR_DIE(SUCCEEDED(result), "Failed to create constant buffer")

    m_gpuBufferByteWidth = byteWidth;
}



//----------------------------------------------------------------------------------------------------------------------
ConstantBuffer::~ConstantBuffer()
{
    ReleaseResources();
}



//----------------------------------------------------------------------------------------------------------------------
void ConstantBuffer::ReleaseResources()
{
    DX_SAFE_RELEASE(m_handle)
}
