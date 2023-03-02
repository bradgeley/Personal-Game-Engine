// Bradley Christensen - 2022-2023
#pragma once
#include "Engine/Core/EngineSubsystem.h"
#include "Engine/Math/Mat44.h"
#include "Engine/Math/Vec3.h"



struct Rgba8;
class Shader;
class Camera;
class Texture;
class VertexBuffer;
class ConstantBuffer;
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
struct CameraConstants
{
    Mat44 m_worldToCamera       = Mat44();  // View Matrix
    Mat44 m_cameraToClip        = Mat44();  // Projection Matrix
};



//----------------------------------------------------------------------------------------------------------------------
struct ModelConstants
{
    Mat44 m_modelMatrix     = Mat44();
    float m_modelRgba[4]    = { 1.f, 1.f, 1.f, 1.f };
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
    
    void SetCameraConstants(CameraConstants const& cameraConstants);
    void SetModelConstants(ModelConstants const& modelConstants);
    void SetModelMatrix(Mat44 const& modelMatrix);
    void SetModelTint(Rgba8 const& modelTint);

    void BindTexture(Texture* texture, int slot = 0);
    void BindShader(Shader* shader);
    void BindRenderTarget(Texture* texture);
    void SetBlendMode(BlendMode blendMode);
    
    void ClearDepth(float depth);
    void BindVertexBuffer(VertexBuffer const* vbo) const;
    void BindConstantBuffer(ConstantBuffer const* cbo, int slot) const;

    ID3D11Device* GetDevice() const;
    ID3D11DeviceContext* GetContext() const;
    
    ID3D11BlendState* CreateBlendState(D3D11_BLEND srcFactor, D3D11_BLEND dstFactor, D3D11_BLEND_OP op);

private:

    void Draw(int vertexCount, int vertexOffset);
    
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
    
    void CreateRasterizerState();
    void DestroyRasterizerState();
    
    void CreateDepthBuffer();
    void DestroyDepthBuffer();
    
    void UpdateRasterizerState();
    void UpdateDepthStencilState();

    void UpdateModelConstants();
    void UpdateCameraConstants();


private:

    RendererConfig const m_config;

    Shader* m_defaultShader = nullptr;
    Shader* m_currentShader = nullptr;
    Texture* m_defaultTexture = nullptr;
    Texture* m_backbufferTexture = nullptr;
	Texture* m_depthBuffer = nullptr;

    // Constant Buffers
    CameraConstants m_cameraConstants;
    ConstantBuffer* m_cameraConstantsGPU = nullptr;
    ModelConstants  m_modelConstants;
    ConstantBuffer* m_modelConstantsGPU = nullptr;
    
    // D3D11 Things
    ID3D11Device* m_device = nullptr;
    ID3D11DeviceContext* m_deviceContext = nullptr;
    IDXGISwapChain* m_swapChain = nullptr;
    ID3D11BlendState* m_blendStateByMode[(size_t) BlendMode::Count] = {};
    ID3D11RasterizerState* m_rasterizerState = nullptr;
	ID3D11DepthStencilState* m_depthStencilState = nullptr;

    // Renderer State
    bool m_isCameraConstantsDirty = true;
    bool m_isModelConstantsDirty = true;
    bool m_isRasterStateDirty = true;
    bool m_isDepthStencilStateDirty = true;
};
