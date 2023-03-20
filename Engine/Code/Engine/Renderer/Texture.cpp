// Bradley Christensen - 2022-2023
#include "Engine/Renderer/Texture.h"
#include "Renderer.h"
#include "Engine/Core/ErrorUtils.h"
#include "Engine/Core/Image.h"
#include "Engine/Renderer/RendererInternal.h" 
#include <cmath>



//----------------------------------------------------------------------------------------------------------------------
Texture::~Texture()
{
    ReleaseResources();
}



//----------------------------------------------------------------------------------------------------------------------
bool Texture::CreateUniformTexture(IntVec2 const& dims, Rgba8 const& tint)
{
    Image image(dims, tint);
    return CreateFromImage(image, false);
}



//----------------------------------------------------------------------------------------------------------------------
bool Texture::CreateFromImage(Image const& image, bool createMipMap)
{
    auto device = g_renderer->GetDevice();
    auto context = g_renderer->GetContext();
    
    m_sourceImagePath = image.GetSourceImagePath();
    m_dimensions = image.GetDimensions();

    D3D11_TEXTURE2D_DESC desc = {};
    desc.Width = m_dimensions.x;
    desc.Height = m_dimensions.y;
    desc.MipLevels = 1;
    desc.ArraySize = 1;
    desc.Usage = D3D11_USAGE_IMMUTABLE;
    desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
    desc.CPUAccessFlags = 0;
    desc.MiscFlags = 0;
    desc.SampleDesc.Count = 1;
    desc.SampleDesc.Quality = 0;

    D3D11_SUBRESOURCE_DATA initialData = {};
    initialData.pSysMem = image.GetRawData();
    initialData.SysMemPitch = 4 * image.GetDimensions().x;
    initialData.SysMemSlicePitch = 0;
    
    HRESULT result = device->CreateTexture2D(&desc, &initialData, &m_textureHandle);
    ASSERT_OR_DIE(SUCCEEDED(result) && m_textureHandle, "Failed to create texture 2d")

    if (createMipMap)
    {
        // Figure out how many levels we need (how many times we have to divide the dims by 2 to get to a 1x1, 1xY or Xx1 texture)
        float smallerOfXandY = static_cast<float>(m_dimensions.x < m_dimensions.y ? m_dimensions.x : m_dimensions.y);
        float mipLevels = std::logf(smallerOfXandY) / std::logf(2.f);
        
        D3D11_TEXTURE2D_DESC mipDesc = {};
        mipDesc.Width = m_dimensions.x;
        mipDesc.Height = m_dimensions.y;
        mipDesc.ArraySize = 1;
        mipDesc.Usage = D3D11_USAGE_DEFAULT;
        mipDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
        mipDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET;
        mipDesc.CPUAccessFlags = 0;
        mipDesc.MiscFlags = D3D11_RESOURCE_MISC_GENERATE_MIPS;
        mipDesc.MipLevels = (UINT) mipLevels;
        mipDesc.SampleDesc.Count = 1;
        mipDesc.SampleDesc.Quality = 0;

        ID3D11Texture2D* mipTextureHandle;
        HRESULT mipResult = device->CreateTexture2D(&mipDesc, nullptr, &mipTextureHandle);
        ASSERT_OR_DIE(SUCCEEDED(mipResult), "Failed to create mip texture")

        context->CopySubresourceRegion(mipTextureHandle, 0, 0, 0, 0, m_textureHandle, 0, nullptr);

        D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
        srvDesc.Format = mipDesc.Format;
        srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
        srvDesc.Texture2D.MipLevels = mipDesc.MipLevels;
        srvDesc.Texture2D.MostDetailedMip = 0;
        HRESULT srvResult = device->CreateShaderResourceView(mipTextureHandle, &srvDesc, &m_shaderResourceView);
        ASSERT_OR_DIE(SUCCEEDED(srvResult), "Failed to create shader resource view for mipped texture.")

        context->GenerateMips(m_shaderResourceView);

        DX_SAFE_RELEASE(m_textureHandle)
        m_textureHandle = mipTextureHandle;
    }
    
    return SUCCEEDED(result);
}



//----------------------------------------------------------------------------------------------------------------------
bool Texture::LoadFromImageFile(const char* imageSource, bool createMipMap)
{
    Image image = Image();
    if (image.LoadFromFile(imageSource))
    {
        return CreateFromImage(image, createMipMap);
    }
    return false;
}



//----------------------------------------------------------------------------------------------------------------------
bool Texture::IsValid() const
{
    return m_textureHandle != nullptr;
}



//----------------------------------------------------------------------------------------------------------------------
IntVec2 Texture::GetDimensions() const
{
    return m_dimensions;
}



//----------------------------------------------------------------------------------------------------------------------
bool Texture::CreateFromSwapChain(IDXGISwapChain* swapChain)
{
    swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**) &m_textureHandle);
    ASSERT_OR_DIE(m_textureHandle != nullptr, "Failed to create texture from swap chain backbuffer")

    D3D11_TEXTURE2D_DESC desc;
    m_textureHandle->GetDesc(&desc);

    m_dimensions = IntVec2((int) desc.Width, (int) desc.Height);
    
    return m_textureHandle != nullptr;
}



//----------------------------------------------------------------------------------------------------------------------
void Texture::ReleaseResources()
{
    DX_SAFE_RELEASE(m_shaderResourceView)
    DX_SAFE_RELEASE(m_renderTargetView)
    DX_SAFE_RELEASE(m_textureHandle)
    DX_SAFE_RELEASE(m_depthStencilView)
}



//----------------------------------------------------------------------------------------------------------------------
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

    HRESULT result = g_renderer->GetDevice()->CreateTexture2D(&desc, nullptr, &texture->m_textureHandle);
    ASSERT_OR_DIE(SUCCEEDED(result), "Failed to create depth buffer")
    return texture;
}



//----------------------------------------------------------------------------------------------------------------------
ID3D11DepthStencilView* Texture::CreateOrGetDepthStencilView()
{
    if (!m_depthStencilView)
    {
        HRESULT result = g_renderer->GetDevice()->CreateDepthStencilView(m_textureHandle, nullptr, &m_depthStencilView);
        ASSERT_OR_DIE(SUCCEEDED(result), "Failed to create depth stencil view")
    }

    return m_depthStencilView;
}



//----------------------------------------------------------------------------------------------------------------------
ID3D11RenderTargetView* Texture::CreateOrGetRenderTargetView()
{
    if (!m_renderTargetView)
    {
        HRESULT result = g_renderer->GetDevice()->CreateRenderTargetView(m_textureHandle, nullptr, &m_renderTargetView);
        ASSERT_OR_DIE(SUCCEEDED(result), "Failed to create rtv")
    }

    return m_renderTargetView;
}



//----------------------------------------------------------------------------------------------------------------------
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
