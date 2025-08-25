// Bradley Christensen - 2022-2023
#include "D3D11Texture.h"
#include "D3D11Internal.h"
#include "D3D11Renderer.h"
#include "D3D11Swapchain.h"
#include "Engine/Core/ErrorUtils.h"
#include "Engine/Core/Image.h"
#include "Engine/Core/StringUtils.h"
#include "Engine/Math/MathUtils.h"



//----------------------------------------------------------------------------------------------------------------------
void D3D11Texture::ReleaseResources()
{
    DX_SAFE_RELEASE(m_shaderResourceView);
    DX_SAFE_RELEASE(m_renderTargetView);
    DX_SAFE_RELEASE(m_textureHandle);
    DX_SAFE_RELEASE(m_depthStencilView);
}



//----------------------------------------------------------------------------------------------------------------------
bool D3D11Texture::IsValid() const
{
    return m_textureHandle != nullptr;
}



//----------------------------------------------------------------------------------------------------------------------
bool D3D11Texture::CreateFromImage(Image const& image, bool createMipMap)
{
    auto device = D3D11Renderer::Get()->GetDevice();
    auto context = D3D11Renderer::Get()->GetDeviceContext();

    m_sourceImagePath = image.GetSourceImagePath();
    m_dimensions = image.GetDimensions();

    D3D11_TEXTURE2D_DESC desc = {};
    desc.Width = m_dimensions.x;
    desc.Height = m_dimensions.y;
    desc.MipLevels = 1;
    desc.ArraySize = 1;
    desc.Usage = D3D11_USAGE_DEFAULT;
    desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    desc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET;
    desc.CPUAccessFlags = 0;
    desc.MiscFlags = 0;
    desc.SampleDesc.Count = 1;
    desc.SampleDesc.Quality = 0;

    D3D11_SUBRESOURCE_DATA initialData = {};
    initialData.pSysMem = image.GetRawData();
    initialData.SysMemPitch = sizeof(Rgba8) * image.GetDimensions().x;
    initialData.SysMemSlicePitch = 0;

    HRESULT result = device->CreateTexture2D(&desc, &initialData, &m_textureHandle);
    ASSERT_OR_DIE(SUCCEEDED(result) && m_textureHandle, "Failed to create texture 2d");

    if (createMipMap)
    {
        // Figure out how many levels we need (how many times we have to divide the dims by 2 to get to a 1x1, 1xY or Xx1 texture)
        float smallerOfXandY = static_cast<float>(m_dimensions.x < m_dimensions.y ? m_dimensions.x : m_dimensions.y);
        float mipLevels = MathUtils::LogF(smallerOfXandY) / MathUtils::LogF(2.f);

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
        ASSERT_OR_DIE(SUCCEEDED(mipResult), "Failed to create mip texture"); 

        context->CopySubresourceRegion(mipTextureHandle, 0, 0, 0, 0, m_textureHandle, 0, nullptr);

        D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
        srvDesc.Format = mipDesc.Format;
        srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
        srvDesc.Texture2D.MipLevels = mipDesc.MipLevels;
        srvDesc.Texture2D.MostDetailedMip = 0;
        HRESULT srvResult = device->CreateShaderResourceView(mipTextureHandle, &srvDesc, &m_shaderResourceView);
        ASSERT_OR_DIE(SUCCEEDED(srvResult), "Failed to create shader resource view for mipped texture.");

        context->GenerateMips(m_shaderResourceView);

        DX_SAFE_RELEASE(m_textureHandle);
        m_textureHandle = mipTextureHandle;
    }

    #ifdef _DEBUG
    std::string name = StringUtils::StringF("Texture (Image). Source: %s", m_sourceImagePath.c_str());
    m_textureHandle->SetPrivateData(WKPDID_D3DDebugObjectName, (int) name.size(), name.data());
    #endif

    return SUCCEEDED(result);
}



//----------------------------------------------------------------------------------------------------------------------
bool D3D11Texture::InitAsBackbufferTexture(IDXGISwapChain* swapChain)
{
    ID3D11Texture2D* swapChainBackBufferTexture = nullptr;
    swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**) &swapChainBackBufferTexture);
    ASSERT_OR_DIE(swapChainBackBufferTexture != nullptr, "Failed to create texture from swap chain backbuffer")

        D3D11_TEXTURE2D_DESC backbufferDesc;
    swapChainBackBufferTexture->GetDesc(&backbufferDesc);

    DX_SAFE_RELEASE(swapChainBackBufferTexture)

        m_dimensions = IntVec2((int) backbufferDesc.Width, (int) backbufferDesc.Height);

    RendererPerUserSettings perUserSettings = g_renderer->GetPerUserSettings();

    // Create our own texture that we will render to, with MSAA enabled

    ID3D11Device* device = D3D11Renderer::Get()->GetDevice();
    MSAASettings msaaSettings = D3D11Renderer::Get()->GetMaxSupportedMSAASettings(DXGI_FORMAT_R8G8B8A8_UNORM);

    D3D11_TEXTURE2D_DESC desc = {};
    desc.Width = m_dimensions.x;
    desc.Height = m_dimensions.y;
    desc.MipLevels = 1;
    desc.ArraySize = 1;
    desc.Usage = D3D11_USAGE_DEFAULT;
    desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    desc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET;
    desc.CPUAccessFlags = 0;
    desc.MiscFlags = 0;
    desc.SampleDesc.Count = msaaSettings.m_sampleCount;
    desc.SampleDesc.Quality = msaaSettings.m_qualityLevel;

    HRESULT result = device->CreateTexture2D(&desc, nullptr, &m_textureHandle);
    ASSERT_OR_DIE(SUCCEEDED(result) && m_textureHandle, "Failed to create texture from swap chain")

        #ifdef _DEBUG
        static int count = 0;
    m_sourceImagePath = StringUtils::StringF("Backbuffer Texture %i", ++count);
    std::string name = StringUtils::StringF("Texture (Image). Source: %s", m_sourceImagePath.c_str());
    m_textureHandle->SetPrivateData(WKPDID_D3DDebugObjectName, (int) name.size(), name.data());
    #endif

    return m_textureHandle != nullptr;
}



//----------------------------------------------------------------------------------------------------------------------
bool D3D11Texture::InitAsDepthBuffer(IDXGISwapChain* swapChain)
{

    ID3D11Texture2D* swapChainBackBufferTexture = nullptr;
    swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**) &swapChainBackBufferTexture);
    ASSERT_OR_DIE(swapChainBackBufferTexture != nullptr, "Failed to create texture from swap chain backbuffer")

        D3D11_TEXTURE2D_DESC backbufferDesc;
    swapChainBackBufferTexture->GetDesc(&backbufferDesc);

    DX_SAFE_RELEASE(swapChainBackBufferTexture)

        m_dimensions = IntVec2((int) backbufferDesc.Width, (int) backbufferDesc.Height);

    RendererPerUserSettings perUserSettings = g_renderer->GetPerUserSettings();
    MSAASettings msaaSettings = D3D11Renderer::Get()->GetMaxSupportedMSAASettings(DXGI_FORMAT_D24_UNORM_S8_UINT);

    D3D11_TEXTURE2D_DESC desc = {};
    desc.Width = m_dimensions.x;
    desc.Height = m_dimensions.y;
    desc.MipLevels = 1;
    desc.ArraySize = 1;
    desc.Usage = D3D11_USAGE_DEFAULT;
    desc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
    desc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
    desc.CPUAccessFlags = 0;
    desc.MiscFlags = 0;
    desc.SampleDesc.Count = msaaSettings.m_sampleCount;
    desc.SampleDesc.Quality = msaaSettings.m_qualityLevel;

    HRESULT result = D3D11Renderer::Get()->GetDevice()->CreateTexture2D(&desc, nullptr, &m_textureHandle);
    ASSERT_OR_DIE(SUCCEEDED(result), "Failed to create depth buffer")

        #ifdef _DEBUG
        static int count = 0;
    m_sourceImagePath = StringUtils::StringF("Depth Buffer %i", ++count);
    std::string name = StringUtils::StringF("Texture (Image). Source: %s", m_sourceImagePath.c_str());
    m_textureHandle->SetPrivateData(WKPDID_D3DDebugObjectName, (int) name.size(), name.data());
    #endif

    return m_textureHandle != nullptr;
}



//----------------------------------------------------------------------------------------------------------------------
void D3D11Texture::CopyTo(Swapchain* swapchain)
{
    D3D11Swapchain* d3dSwapchain = dynamic_cast<D3D11Swapchain*>(swapchain);
    ASSERT_OR_DIE(d3dSwapchain && d3dSwapchain->m_swapChain, "Invalid swapchain.");

    ID3D11Texture2D* swapChainBackBufferTexture = nullptr;
    HRESULT hr = d3dSwapchain->m_swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), reinterpret_cast<void**>(&swapChainBackBufferTexture));
    ASSERT_OR_DIE(SUCCEEDED(hr), "Failed to get swapchain buffer.");

    ID3D11DeviceContext* context = D3D11Renderer::Get()->GetDeviceContext();

    // Paint onto the actual backbuffer texture using the texture that we've been rendering to at higher quality (lets us use MSAA)
    context->ResolveSubresource(
        swapChainBackBufferTexture, // actual backbuffer d3d texture
        0,
        m_textureHandle, // higher sample count texture that we've been rendering to
        0,
        DXGI_FORMAT_R8G8B8A8_UNORM
    );

    DX_SAFE_RELEASE(swapChainBackBufferTexture);
}



//----------------------------------------------------------------------------------------------------------------------
ID3D11DepthStencilView* D3D11Texture::CreateOrGetDepthStencilView()
{
    if (!m_depthStencilView)
    {
        ID3D11Device* device = D3D11Renderer::Get()->GetDevice();
        HRESULT result = device->CreateDepthStencilView(m_textureHandle, nullptr, &m_depthStencilView);
        ASSERT_OR_DIE(SUCCEEDED(result), "Failed to create depth stencil view")

            #ifdef _DEBUG
            std::string name = StringUtils::StringF("Texture (Depth Stencil View). Source Image: %s", m_sourceImagePath.c_str());
        m_depthStencilView->SetPrivateData(WKPDID_D3DDebugObjectName, (int) name.size(), name.data());
        #endif
    }

    return m_depthStencilView;
}



//----------------------------------------------------------------------------------------------------------------------
ID3D11RenderTargetView* D3D11Texture::CreateOrGetRenderTargetView()
{
    if (!m_renderTargetView)
    {
        ID3D11Device* device = D3D11Renderer::Get()->GetDevice();
        HRESULT result = device->CreateRenderTargetView(m_textureHandle, nullptr, &m_renderTargetView);
        ASSERT_OR_DIE(SUCCEEDED(result), "Failed to create rtv")

            #ifdef _DEBUG
            std::string name = StringUtils::StringF("Texture (Render Target View). Source Image: %s", m_sourceImagePath.c_str());
        m_renderTargetView->SetPrivateData(WKPDID_D3DDebugObjectName, (int) name.size(), name.data());
        #endif
    }

    return m_renderTargetView;
}



//----------------------------------------------------------------------------------------------------------------------
ID3D11ShaderResourceView* D3D11Texture::CreateOrGetShaderResourceView()
{
    if (!m_shaderResourceView)
    {
        ID3D11Device* device = D3D11Renderer::Get()->GetDevice();
        HRESULT result = device->CreateShaderResourceView(m_textureHandle, nullptr, &m_shaderResourceView);
        ASSERT_OR_DIE(SUCCEEDED(result), "Failed to create srv")

            #ifdef _DEBUG
            std::string name = StringUtils::StringF("Texture (Shader Resource View). Source Image: %s", m_sourceImagePath.c_str());
        m_shaderResourceView->SetPrivateData(WKPDID_D3DDebugObjectName, (int) name.size(), name.data());
        #endif
    }

    return m_shaderResourceView;
}

