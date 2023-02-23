// Bradley Christensen - 2022
#include "Engine/Renderer/Texture.h"
#include "Renderer.h"
#include "Engine/Core/ErrorUtils.h"
#include "Engine/Renderer/RendererInternal.h" 



void Texture::WatchInternal(ID3D11Texture2D* handle)
{
    ReleaseResources();

    m_textureHandle = handle;
    if (m_textureHandle)
    {
        m_textureHandle->AddRef();

        D3D11_TEXTURE2D_DESC desc;
        handle->GetDesc(&desc);

        m_dimensions = IntVec2((int) desc.Width, (int) desc.Height);
    }
}



void Texture::ReleaseResources()
{
    DX_SAFE_RELEASE(m_shaderResourceView)
    DX_SAFE_RELEASE(m_renderTargetView)
    DX_SAFE_RELEASE(m_textureHandle)
    DX_SAFE_RELEASE(m_depthStencilView)
}



Texture* Texture::CreateDepthBuffer(IntVec2 const& texelSize)
{
    Texture* texture = new Texture();

    D3D11_TEXTURE2D_DESC desc = {};
    desc.Width = texelSize.x;
    desc.Height = texelSize.y;
    desc.MipLevels = 1;
    desc.ArraySize = 1;
    desc.Usage = D3D11_USAGE_DEFAULT; // GPU WRITABLE
    desc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
    desc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
    desc.CPUAccessFlags = 0;
    desc.MiscFlags = 0;
    desc.SampleDesc.Count = 1;
    desc.SampleDesc.Quality = 0;

    HRESULT result = g_renderer->GetDevice()->CreateTexture2D( &desc, nullptr, &texture->m_textureHandle );
    ASSERT_OR_DIE(SUCCEEDED(result), "Failed to create depth buffer")
    return texture;
}



ID3D11DepthStencilView* Texture::CreateOrGetDepthStencilView()
{
    if (!m_depthStencilView)
    {
        HRESULT result = g_renderer->GetDevice()->CreateDepthStencilView(m_textureHandle, nullptr, &m_depthStencilView);
        ASSERT_OR_DIE(SUCCEEDED(result), "Failed to create depth stencil view")
    }

    return m_depthStencilView;
}



ID3D11RenderTargetView* Texture::CreateOrGetRenderTargetView()
{
    if (!m_renderTargetView)
    {
        HRESULT result = g_renderer->GetDevice()->CreateRenderTargetView(m_textureHandle, nullptr, &m_renderTargetView);
        ASSERT_OR_DIE(SUCCEEDED(result), "Failed to create rtv")
    }

    return m_renderTargetView;
}



ID3D11ShaderResourceView* Texture::CreateOrGetShaderResourceView()
{
    if (!m_shaderResourceView)
    {
        ID3D11Device* device = g_renderer->GetDevice();
        HRESULT result = device->CreateShaderResourceView(m_textureHandle, nullptr, &m_shaderResourceView);
        ASSERT_OR_DIE(SUCCEEDED(result), "Failed to create srv")
    }

    return m_shaderResourceView;
}
