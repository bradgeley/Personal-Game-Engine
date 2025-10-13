// Bradley Christensen - 2022-2025
#pragma once
#include "Game/Framework/EngineBuildPreferences.h"

#if defined(RENDERER_D3D11)

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
    virtual void BindVertexBuffer(VertexBufferID vbo) const override;
    virtual void BindVertexBuffer(VertexBuffer& vbo) const override;
	virtual void BindInstanceBuffer(InstanceBufferID ibo) const override;
	virtual void BindInstanceBuffer(InstanceBuffer& ibo) const override;
    virtual void BindConstantBuffer(ConstantBufferID cbo, int slot) const override;

    // Factory Functions
    virtual TextureID MakeTexture() override;
    virtual ShaderID MakeShader(ShaderConfig const& config) override;
    virtual ConstantBufferID MakeConstantBuffer(size_t initialSize) override;
    virtual VertexBufferID MakeVertexBuffer() override;
    virtual InstanceBufferID MakeInstanceBuffer() override;
    virtual SwapchainID MakeSwapchain() override;
    virtual RenderTargetID MakeSwapchainRenderTarget(void* hwnd, IntVec2 const& resolution) override;
    virtual GPUBuffer* MakeGPUBuffer(GpuBufferConfig const& config) override;    // Does not own lifetime of this buffer

    ID3D11Device* GetDevice() const;
    ID3D11DeviceContext* GetDeviceContext() const;

    virtual void MSAAChanged() override;

    virtual void BindRenderTarget(RenderTargetID renderTargetID) override;
    virtual void ResizeSwapChainRenderTarget(RenderTargetID renderTargetID, IntVec2 const& newSize) override;
    virtual bool SetFullscreenState(RenderTargetID renderTargetID, bool fullscreen) override;

    MSAASettings GetMaxSupportedMSAASettings(DXGI_FORMAT format);

private:

    virtual void Draw(int vertexCount, int vertexOffset) override;
	virtual void DrawInstanced(int vertexCount, int instanceCount, int vertexOffset, int instanceOffset) override;

    // Deferred pipeline updates
    virtual void RasterizerStateUpdated() override;
    virtual void DepthStencilStateUpdated() override;
    virtual void BlendModeUpdated() override;
    virtual void SamplerStateUpdated() override;
    virtual void BoundTextureUpdated(int slot = 0) override;
    virtual void BoundShaderUpdated() override;

    // State creation
    virtual void CreateDebugLayer() override;
    virtual void CreateDevice() override;
    virtual void CreateBlendStates() override;
    virtual void CreateRasterizerState() override;
    virtual void CreateDefaultShader() override;
    virtual void CreateDefaultFont() override;

    // D3D11 Specific State Creation
    ID3D11BlendState* CreateBlendState(BlendMode blendMode);
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

#endif // RENDERER_D3D11