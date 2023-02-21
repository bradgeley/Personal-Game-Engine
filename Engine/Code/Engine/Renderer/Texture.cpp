// Bradley Christensen - 2022
#include "Engine/Renderer/Texture.h"
#include "Renderer.h"
#include "Engine/Core/ErrorUtils.h"
#include "Engine/Renderer/RendererInternal.h" 



void Texture::WatchInternal(ID3D11Texture2D* handle)
{
    ReleaseResources();

    m_textureHandle = handle;
    if ( m_textureHandle )
    {
        m_textureHandle->AddRef();

        D3D11_TEXTURE2D_DESC desc;
        handle->GetDesc( &desc );

        m_dimensions = IntVec2( (int) desc.Width, (int) desc.Height );
    }
}



void Texture::ReleaseResources()
{
    DX_SAFE_RELEASE(m_shaderResourceView)
    DX_SAFE_RELEASE(m_renderTargetView)
    DX_SAFE_RELEASE(m_textureHandle)
    DX_SAFE_RELEASE(m_depthStencilView)
}



ID3D11DepthStencilView* Texture::GetOrCreateDepthStencilView()
{
    ASSERT_OR_DIE( g_renderer && g_renderer->GetDevice(), "Cant create a texture with a null src/device" )

    if ( !m_depthStencilView )
    {
        HRESULT result = g_renderer->GetDevice()->CreateDepthStencilView( m_textureHandle, nullptr, &m_depthStencilView );
        ASSERT_OR_DIE( SUCCEEDED( result ), "Failed to create rtv" )
    }

    return m_depthStencilView;
}



ID3D11RenderTargetView* Texture::GetOrCreateRenderTargetView()
{
    ASSERT_OR_DIE( g_renderer && g_renderer->GetDevice(), "Cant create a texture with a null src/device" )

    if ( !m_renderTargetView )
    {
        HRESULT result = g_renderer->GetDevice()->CreateRenderTargetView( m_textureHandle, nullptr, &m_renderTargetView );
        ASSERT_OR_DIE( SUCCEEDED( result ), "Failed to create rtv" )
    }

    return m_renderTargetView;
}



ID3D11ShaderResourceView* Texture::GetOrCreateShaderResourceView()
{
    ASSERT_OR_DIE( g_renderer && g_renderer->GetDevice(), "Renderer/Device is null" )

    if ( !m_shaderResourceView )
    {
        ID3D11Device* device = g_renderer->GetDevice();
        HRESULT result = device->CreateShaderResourceView( m_textureHandle, nullptr, &m_shaderResourceView );
        ASSERT_OR_DIE( SUCCEEDED( result ), "Failed to create srv" )
    }

    return m_shaderResourceView;
}
