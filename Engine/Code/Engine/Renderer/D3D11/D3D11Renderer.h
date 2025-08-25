// Bradley Christensen - 2022-2023
#pragma once
#include "Engine/Renderer/Renderer.h"
#include "D3D11Utils.h"
#include <unordered_map>



struct D3D11SwapchainRenderTarget;
struct NamedProperties;



//----------------------------------------------------------------------------------------------------------------------
// D3D11 Renderer
//
class D3D11Renderer : public Renderer
{
public:

    D3D11Renderer(RendererConfig const& config);
    static D3D11Renderer* Get();

    virtual void Present() const override;

    virtual void ClearScreen(Rgba8 const& tint) override;
    virtual void ClearDepth(float depth) override;
    virtual void BindVertexBuffer(VertexBuffer const* vbo) const override;
    virtual void BindConstantBuffer(ConstantBuffer const* cbo, int slot) const override;

    // Factory Functions
    virtual Texture* MakeTexture() const override;
    virtual Shader* MakeShader(ShaderConfig const& config) const override;
    virtual ConstantBuffer* MakeConstantBuffer() const override;
    virtual VertexBuffer* MakeVertexBuffer() const override;
    virtual Swapchain* MakeSwapchain() const override;
    virtual RenderTargetID MakeSwapchainRenderTarget(void* hwnd, IntVec2 const& resolution) override;

    // ID Functions
    RenderTargetID RequestRenderTargetID() const;

    // Release Functions
    virtual void ReleaseSwapchainRenderTarget(RenderTargetID renderTargetID) override;

    ID3D11Device* GetDevice() const;
    ID3D11DeviceContext* GetDeviceContext() const;

    virtual void BindRenderTarget(RenderTargetID renderTargetID) override;
    virtual void ResizeSwapChainRenderTarget(RenderTargetID renderTargetID, IntVec2 const& newSize) override;
    virtual bool SetFullscreenState(RenderTargetID renderTargetID, bool fullscreen) override;

    MSAASettings GetMaxSupportedMSAASettings(DXGI_FORMAT format);

private:

    virtual void Draw(int vertexCount, int vertexOffset) override;

    // Deferred pipeline updates
    virtual void RasterizerStateUpdated() override;
    virtual void DepthStencilStateUpdated() override;
    virtual void BlendModeUpdated() override;
    virtual void SamplerStateUpdated() override;
    virtual void BoundTextureUpdated() override;
    virtual void BoundShaderUpdated() override;

    // State creation
    virtual void CreateDebugLayer() override;
    virtual void CreateDevice() override;
    virtual void CreateBlendStates() override;
    virtual void CreateRasterizerState() override;
    virtual void CreateDefaultShader() override;
    virtual void CreateDefaultFont() override;

    // D3D11 Specific State Creation
    ID3D11BlendState* CreateBlendState(D3D11_BLEND srcFactor, D3D11_BLEND dstFactor, D3D11_BLEND_OP op);
    ID3D11SamplerState* CreateSamplerState(SamplerFilter filter, SamplerAddressMode addressMode);

    // State cleanup
    virtual void DestroyDebugLayer() override;
    virtual void DestroyDevice() override;
    virtual void DestroyBlendStates() override;
    virtual void DestroyDepthStencilState() override;
    virtual void DestroySamplerStates() override;
    virtual void DestroyRasterizerState() override;

    // Debug
    void ReportLiveObjects();

private:

    // Dev Console Commands
    virtual bool ToggleMSAA(NamedProperties& args);

protected:


    ID3D11Device* m_device = nullptr;
    ID3D11DeviceContext* m_deviceContext = nullptr;

    ID3D11BlendState* m_blendStateByMode[(size_t) BlendMode::Count] = {};
    ID3D11SamplerState* m_samplerState = nullptr;
    ID3D11RasterizerState* m_rasterizerState = nullptr;
    ID3D11DepthStencilState* m_depthStencilState = nullptr;

#if defined(_DEBUG)
    void* m_debugModule = nullptr;
    IDXGIDebug* m_debug = nullptr;
#endif
};

