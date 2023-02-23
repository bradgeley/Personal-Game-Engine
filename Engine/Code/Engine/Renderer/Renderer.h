// Bradley Christensen - 2022
#pragma once
#include "Engine/Core/EngineSubsystem.h"


struct Rgba8;
class Shader;
class Camera;
class Texture;
class VertexBuffer;
struct IDXGISwapChain;
struct ID3D11Device;
struct ID3D11DeviceContext;
struct ID3D11DepthStencilView;
struct ID3D11BlendState;
struct ID3D11RasterizerState;
struct ID3D11DepthStencilState;
enum   D3D11_BLEND : int;
enum   D3D11_BLEND_OP : int;



extern class Renderer* g_renderer;



//----------------------------------------------------------------------------------------------------------------------
enum class BlendMode
{
    Opaque = 0,
    Alpha,
    Additive,

    Count
};



//----------------------------------------------------------------------------------------------------------------------
struct RendererConfig
{
    
};



//----------------------------------------------------------------------------------------------------------------------
// Renderer
//
// The game's renderer! Make's triangles show up and stuff.
//
class Renderer : public EngineSubsystem
{
public:

    Renderer(RendererConfig const& config);
    
    virtual void Startup() override;
    virtual void Shutdown() override;
    virtual void BeginFrame() override;
    virtual void EndFrame() override;
    
    void BeginCamera(Camera const& camera);
    void EndCamera(Camera const& camera);

    void ClearScreen(Rgba8 const& tint);
    void DrawVertexBuffer(VertexBuffer* vbo);

    void BindShader(Shader* shader);
    void BindRenderTarget(Texture* texture);
    void SetBlendMode(BlendMode blendMode);

    ID3D11Device* GetDevice() const;
    ID3D11DeviceContext* GetContext() const;
    
    ID3D11BlendState* CreateBlendState(D3D11_BLEND srcFactor, D3D11_BLEND dstFactor, D3D11_BLEND_OP op);

private:

    void Draw(int vertexCount, int vertexOffset);
    void CreateRenderContext();
    void CreateDefaultShader();
    void CreateRasterizerState();
    void CreateDepthBuffer();
    void ClearDepth(float depth);
    void DestroyRenderContext();
    void CreateBlendStates();
    void DestroyBlendStates();
    void UpdateRasterizerState();
    void UpdateDepthStencilState();
    void BindVertexBuffer(VertexBuffer const* vbo) const;

private:

    RendererConfig const m_config;

    Shader* m_defaultShader = nullptr;
    Shader* m_currentShader = nullptr;
    Texture* m_backbufferTexture = nullptr;
	Texture* m_depthBuffer = nullptr;
    
    // D3D11 Things
    ID3D11Device* m_device = nullptr;
    ID3D11DeviceContext* m_deviceContext = nullptr;
    IDXGISwapChain* m_swapChain = nullptr;
    ID3D11BlendState* m_blendStateByMode[(size_t) BlendMode::Count] = {};
    ID3D11RasterizerState* m_rasterizerState = nullptr;
	ID3D11DepthStencilState* m_depthStencilState = nullptr;

    bool m_isRasterStateDirty = true;
    bool m_isDepthStencilStateDirty = true;
};
