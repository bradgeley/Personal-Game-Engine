// Bradley Christensen - 2022-2023
#pragma once
#include "Engine/Core/EngineSubsystem.h"
#include "RendererSettings.h"
#include <unordered_map>



//----------------------------------------------------------------------------------------------------------------------
class Camera;
class ConstantBuffer;
class Font;
class Shader;
class Texture;
class Swapchain;
class VertexBuffer;
class Window;
struct IntVec2;
struct NamedProperties;
struct RenderTarget;
struct ShaderConfig;



//----------------------------------------------------------------------------------------------------------------------
typedef uint32_t RenderTargetID;
extern uint32_t INVALID_RENDER_TARGET_ID;



//----------------------------------------------------------------------------------------------------------------------
// THE Renderer
//
extern class RendererInterface* g_rendererInterface;



//----------------------------------------------------------------------------------------------------------------------
struct RendererConfig
{

};



//----------------------------------------------------------------------------------------------------------------------
// RendererInterface
//
// The game's renderer! Hardware/Platform abstracted
//
class RendererInterface : public EngineSubsystem
{
public:

    RendererInterface(RendererConfig const& config);
    
    virtual void Startup() override;
    virtual void Render() const override;
    virtual void Shutdown() override;
    virtual void BeginFrame() override;
    virtual void EndFrame() override;

    void BeginWindow(Window const* window);
    void EndWindow(Window const* window);

    void BeginCamera(Camera const* camera);
    void EndCamera(Camera const* camera);
    Camera const* GetCurrentCamera() const;

    void BeginCameraAndWindow(Camera const* camera, Window const* window);
    void EndCameraAndWindow(Camera const* camera, Window const* window);

    virtual void Present() const = 0;

    RendererPerUserSettings GetPerUserSettings() const;

    virtual void ClearScreen(Rgba8 const& tint) = 0;
    void DrawVertexBuffer(VertexBuffer* vbo);

    virtual void ClearDepth(float depth) = 0;
    virtual void BindVertexBuffer(VertexBuffer const* vbo) const = 0;
    virtual void BindConstantBuffer(ConstantBuffer const* cbo, int slot) const = 0;

    // Factory Functions
    static RendererInterface* MakeRendererInterface(RendererConfig const& config);
    virtual Texture* MakeTexture() const = 0;
    virtual Shader* MakeShader(ShaderConfig const& config) const = 0;
    virtual ConstantBuffer* MakeConstantBuffer() const = 0;
    virtual VertexBuffer* MakeVertexBuffer() const = 0;
    virtual Swapchain* MakeSwapchain() const = 0;
    virtual RenderTargetID MakeSwapchainRenderTarget(void* hwnd, IntVec2 const& initialDims) = 0;

    // Release Functions
    virtual void ReleaseSwapchainRenderTarget(RenderTargetID renderTarget) = 0;

    // Rendering Pipeline State
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
    virtual void BindRenderTarget(RenderTargetID renderTarget) = 0;
    virtual void ResizeSwapChainRenderTarget(RenderTargetID renderTarget, IntVec2 const& newSize) = 0;
    virtual void UnbindRenderTarget(RenderTargetID renderTarget);
    virtual bool SetFullscreenState(RenderTargetID renderTarget, bool fullscreen) = 0;

    int GetNumFrameDrawCalls() const;
    Font* GetDefaultFont() const;

protected:

    virtual void Draw(int vertexCount, int vertexOffset);

    // Deferred pipeline updates
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

    // Deferred pipeline updates, per renderer type
    virtual void RasterizerStateUpdated() = 0;
    virtual void DepthStencilStateUpdated() = 0;
    virtual void ModelConstantsUpdated();
    virtual void CameraConstantsUpdated();
    virtual void FontConstantsUpdated();
    virtual void BlendModeUpdated() = 0;
    virtual void SamplerStateUpdated() = 0;
    virtual void BoundTextureUpdated() = 0;
    virtual void BoundShaderUpdated() = 0;

    // State creation
    virtual void CreateDebugLayer() = 0;
    virtual void CreateDevice() = 0;
    virtual void CreateConstantBuffers();
    virtual void CreateBlendStates() = 0;
    virtual void CreateRasterizerState() = 0;
    virtual void CreateDefaultShader() = 0;
    virtual void CreateDefaultTexture();
    virtual void CreateDefaultFont() = 0;

    // State cleanup
    virtual void DestroyDebugLayer() = 0;
    virtual void DestroyDevice() = 0;
    virtual void DestroyConstantBuffers();
    virtual void DestroyBlendStates() = 0;
    virtual void DestroyDepthStencilState() = 0;
    virtual void DestroySamplerStates() = 0;
    virtual void DestroyRasterizerState() = 0;
    virtual void DestroyDefaultShader();
    virtual void DestroyDefaultTexture();
    virtual void DestroyDefaultFont();

    // Dev Console Commands
    void AddDevConsoleCommands();
    void RemoveDevConsoleCommands();
    bool DebugDrawVertexBuffers(NamedProperties& args);
    bool ToggleVSync(NamedProperties& args);
    virtual bool ToggleMSAA(NamedProperties& args) = 0;

protected:

    // Configuration
    RendererConfig const m_config;
    RendererPerUserSettings m_perUserSettings;

    // Renderer Pipeline State
    RenderTargetID m_currentRenderTarget = INVALID_RENDER_TARGET_ID;
    Camera const* m_currentCamera = nullptr;
    RendererSettings m_settings;
    RendererSettings m_dirtySettings;
    int m_numFrameDrawCalls = 0;

    // Constant Buffers
    ConstantBuffer* m_cameraConstantsGPU = nullptr;
    ConstantBuffer* m_modelConstantsGPU = nullptr;
    ConstantBuffer* m_fontConstantsGPU = nullptr;

    // Defaults
    Shader* m_defaultShader = nullptr;
    Texture* m_defaultTexture = nullptr;
    Font* m_defaultFont = nullptr;

    std::unordered_map<RenderTargetID, RenderTarget*> m_renderTargets;

    // Debug
#if defined(_DEBUG)
    bool m_debugDrawVertexBuffers = false;
#endif
};

