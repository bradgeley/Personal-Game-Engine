// Bradley Christensen - 2022-2023
#include "D3D11ConstantBuffer.h"
#include "D3D11Internal.h"
#include "D3D11Renderer.h"
#include "Engine/Core/ErrorUtils.h"



//----------------------------------------------------------------------------------------------------------------------
void D3D11ConstantBuffer::ReleaseResources()
{
    DX_SAFE_RELEASE(m_handle)
}



//----------------------------------------------------------------------------------------------------------------------
void D3D11ConstantBuffer::Update(void const* data, size_t byteWidth)
{
    if (!m_handle || byteWidth > m_gpuBufferByteWidth)
    {
        Initialize(byteWidth);
    }

    // GPU
    D3D11_MAPPED_SUBRESOURCE mapping;
    HRESULT result = D3D11Renderer::Get()->GetDeviceContext()->Map(
        m_handle,
        0,
        D3D11_MAP_WRITE_DISCARD,
        0,
        &mapping
    );

    ASSERT_OR_DIE(SUCCEEDED(result), "Failed to map data for constant buffer")

    memcpy(mapping.pData, data, byteWidth);

    D3D11Renderer::Get()->GetDeviceContext()->Unmap(m_handle, 0);
}



//----------------------------------------------------------------------------------------------------------------------
void D3D11ConstantBuffer::Initialize(size_t byteWidth)
{
    ReleaseResources();

    D3D11_BUFFER_DESC desc;
    desc.ByteWidth = (UINT) byteWidth;
    desc.Usage = D3D11_USAGE_DYNAMIC;
    desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
    desc.MiscFlags = 0;
    desc.StructureByteStride = 0;

    HRESULT result = D3D11Renderer::Get()->GetDevice()->CreateBuffer(&desc, nullptr, &m_handle);
    ASSERT_OR_DIE(SUCCEEDED(result), "Failed to create constant buffer")

    m_gpuBufferByteWidth = byteWidth;

    #if defined(_DEBUG)
        m_handle->SetPrivateData(WKPDID_D3DDebugObjectName, (UINT) strlen("D3D11ConstantBuffer"), "D3D11ConstantBuffer");
    #endif
}
