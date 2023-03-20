// Bradley Christensen - 2022-2023
#pragma once
#include "Engine/Core/EngineSubsystem.h"
#include "Engine/Renderer/EngineConstantBuffers.h"
#include "Engine/Renderer/RendererSettings.h"
#include "Engine/Renderer/D3D11Utils.h"



//----------------------------------------------------------------------------------------------------------------------
// THE Renderer
//
extern class Renderer* g_renderer;
//



//----------------------------------------------------------------------------------------------------------------------
struct Rgba8;
class Font;
class Shader;
class Camera;
class Texture;
class VertexBuffer;
class ConstantBuffer;



//----------------------------------------------------------------------------------------------------------------------
struct RendererConfig
{
    
};



//----------------------------------------------------------------------------------------------------------------------
// Renderer
//
// The game's renderer! Make's triangles show up and stuff.
//
// Keeps track of current pipeline state in RendererSettings, keeping new changes in a copy called "dirtySettings"
// Only updates renderer state, if necessary, just before a Draw call - in order to minimize the number of gpu syncs
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
    
    void ResetRenderingPipelineState();
    void SetCameraConstants(CameraConstants const& cameraConstants);
    void SetModelConstants(ModelConstants const& modelConstants);
    void SetFontConstants(FontConstants const& fontConstants);
    void SetModelMatrix(Mat44 const& modelMatrix);
    void SetModelTint(Rgba8 const& modelTint);
    void SetBlendMode(BlendMode blendMode);
    void SetSamplerMode(SamplerFilter filter, SamplerAddressMode addressMode);
    void SetCullMode(CullMode cullMode);
    void SetWindingOrder(Winding winding);
    void SetFillMode(FillMode fillMode);

    void BindTexture(Texture* texture);
    void BindShader(Shader* shader);
    void BindRenderTarget(Texture* texture);
    
    void ClearDepth(float depth);
    void BindVertexBuffer(VertexBuffer const* vbo) const;
    void BindConstantBuffer(ConstantBuffer const* cbo, int slot) const;

    Font* GetDefaultFont() const; // todo: move somewhere else?

    ID3D11Device* GetDevice() const;
    ID3D11DeviceContext* GetContext() const;
    
    ID3D11BlendState* CreateBlendState(D3D11_BLEND srcFactor, D3D11_BLEND dstFactor, D3D11_BLEND_OP op);

private:

    void Draw(int vertexCount, int vertexOffset);
    
    void CreateDebugLayer();
    void DestroyDebugLayer();
    void ReportLiveObjects();
    
    ID3D11SamplerState* CreateSamplerState(SamplerFilter filter, SamplerAddressMode addressMode);
    void DestroySamplerStates();
    
    void CreateRenderContext();
    void DestroyRenderContext();
    
    void CreateConstantBuffers();
    void DestroyConstantBuffers();
    
    void CreateBlendStates();
    void DestroyBlendStates();
    
    void CreateDefaultShader();
    void DestroyDefaultShader();
    
    void CreateDefaultTexture();
    void DestroyDefaultTexture();
    
    void CreateDefaultFont();
    void DestroyDefaultFont();
    
    void CreateRasterizerState();
    void DestroyRasterizerState();
    
    void CreateDepthBuffer();
    void DestroyDepthBuffer();
    void DestroyDepthStencilState();

    void UpdateRenderingPipelineState(bool force = false);
    void UpdateRasterizerState(bool force = false);
    void UpdateDepthStencilState(bool force = false);
    void UpdateModelConstants(bool force = false);
    void UpdateCameraConstants(bool force = false);
    void UpdateFontConstants(bool force = false);
    void UpdateBlendMode(bool force = false);
    void UpdateSamplerState(bool force = false);
    void UpdateTexture(bool force = false);
    void UpdateShader(bool force = false);

private:

    RendererConfig const m_config;

    Shader* m_defaultShader = nullptr;
    Texture* m_defaultTexture = nullptr;
    Font* m_defaultFont = nullptr;
    Texture* m_backbufferTexture = nullptr;
	Texture* m_depthBuffer = nullptr;

    // Constant Buffers
    ConstantBuffer* m_cameraConstantsGPU = nullptr;
    ConstantBuffer* m_modelConstantsGPU = nullptr;
    ConstantBuffer* m_fontConstantsGPU = nullptr;
    
    // D3D11 Things
    ID3D11Device* m_device = nullptr;
    ID3D11DeviceContext* m_deviceContext = nullptr;
    IDXGISwapChain* m_swapChain = nullptr;
    ID3D11BlendState* m_blendStateByMode[(size_t) BlendMode::Count] = {};
	ID3D11SamplerState* m_samplerState = nullptr;
    ID3D11RasterizerState* m_rasterizerState = nullptr;
	ID3D11DepthStencilState* m_depthStencilState = nullptr;

    // Renderer Pipeline State
    // todo: add more things to this
    RendererSettings m_settings;
    RendererSettings m_dirtySettings;

#if defined(_DEBUG)
    void* m_debugModule = nullptr;
    IDXGIDebug* m_debug = nullptr;
#endif
};
