// Bradley Christensen - 2022-2025
#pragma once
#include "Engine/Core/EngineSubsystem.h"
#include "RendererSettings.h"
#include "RendererUtils.h"
#include <unordered_map>
#include <mutex>



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
// THE Renderer
//
extern class Renderer* g_renderer;



//----------------------------------------------------------------------------------------------------------------------
struct RendererConfig
{
	RendererUserSettings m_startupUserSettings;
};



//----------------------------------------------------------------------------------------------------------------------
// RendererInterface
//
// The game's renderer! Hardware/Platform abstracted
//
class Renderer : public EngineSubsystem
{
public:

    Renderer(RendererConfig const& config);
    Renderer(Renderer const& other) = delete;
    virtual ~Renderer() override;
    
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

    RendererUserSettings GetPerUserSettings() const;

    virtual void ClearScreen(Rgba8 const& tint) = 0;
    void DrawVertexBuffer(VertexBufferID id);
    void DrawVertexBuffer(VertexBuffer& vbo);

    virtual void ClearDepth(float depth) = 0;

    // Factory Make Functions
    virtual TextureID           MakeTexture() = 0;
    virtual ShaderID            MakeShader(ShaderConfig const& config) = 0;
    virtual ConstantBufferID    MakeConstantBuffer() = 0;
    virtual VertexBufferID      MakeVertexBuffer() = 0;
    virtual SwapchainID         MakeSwapchain() = 0;
    virtual RenderTargetID      MakeSwapchainRenderTarget(void* hwnd, IntVec2 const& initialDims) = 0;
    virtual FontID              MakeFont();

    // Factory Get Functions
    Texture*                    GetTexture(TextureID id) const;
    Shader*                     GetShader(ShaderID id) const;
    ConstantBuffer*             GetConstantBuffer(ConstantBufferID id) const;
    VertexBuffer*               GetVertexBuffer(VertexBufferID id) const;
    Swapchain*                  GetSwapchain(SwapchainID id) const;
    RenderTarget*               GetRenderTarget(RenderTargetID id);
    Font*                       GetFont(FontID id);

    // Factory Release Functions
    void                        ReleaseTexture(TextureID id);
    void                        ReleaseShader(ShaderID id);
    void                        ReleaseConstantBuffer(ConstantBufferID id);
    void                        ReleaseVertexBuffer(VertexBufferID id);
    void                        ReleaseSwapchain(SwapchainID id);
    void                        ReleaseRenderTarget(RenderTargetID id);
    void                        ReleaseFont(FontID id);

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

    // Binding
    void BindShader(Shader* shader);
    void BindShader(ShaderID shader);
    void BindTexture(Texture* texture);
    void BindTexture(TextureID texture);
    virtual void BindRenderTarget(RenderTargetID renderTarget) = 0;
    virtual void BindVertexBuffer(VertexBufferID vbo) const = 0;
    virtual void BindVertexBuffer(VertexBuffer& vbo) const = 0;
    virtual void BindConstantBuffer(ConstantBufferID cbo, int slot) const = 0;

    virtual void ResizeSwapChainRenderTarget(RenderTargetID renderTarget, IntVec2 const& newSize) = 0;
    virtual void UnbindRenderTarget(RenderTargetID renderTarget);
    virtual bool SetFullscreenState(RenderTargetID renderTarget, bool fullscreen) = 0;

    int GetNumFrameDrawCalls() const;

    // Get Defaults
    Font* GetDefaultFont() const;
    Texture* GetDefaultTexture() const;
    Shader* GetDefaultShader() const;

    // Get Bound
    Shader* GetBoundShader() const;
    Texture* GetBoundTexture() const;

protected:

    // ID Functions
    TextureID        RequestTextureID() const;
    ShaderID         RequestShaderID() const;
    ConstantBufferID RequestConstantBufferID() const;
    VertexBufferID   RequestVertexBufferID() const;
    SwapchainID      RequestSwapchainID() const;
    RenderTargetID   RequestRenderTargetID() const;
    FontID           RequestFontID() const;

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
    virtual void DestroyBlendStates() = 0;
    virtual void DestroyDepthStencilState() = 0;
    virtual void DestroySamplerStates() = 0;
    virtual void DestroyRasterizerState() = 0;

    // Factory cleanup
    virtual void DestroyShaders();
    virtual void DestroyTextures();
    virtual void DestroyConstantBuffers();
    virtual void DestroyVertexBuffers();
    virtual void DestroySwapchains();
    virtual void DestroyRenderTargets();
    virtual void DestroyFonts();

    // Dev Console Commands
    void AddDevConsoleCommands();
    void RemoveDevConsoleCommands();
    bool DebugDrawVertexBuffers(NamedProperties& args);
    bool ToggleVSync(NamedProperties& args);
    virtual bool ToggleMSAA(NamedProperties& args) = 0;

protected:

    // Configuration
    RendererConfig const m_config;
    RendererUserSettings m_userSettings;

    // Renderer Pipeline State
    RenderTargetID m_currentRenderTarget       = RendererUtils::InvalidID;
    Camera const* m_currentCamera = nullptr;
    RendererSettings m_settings;
    RendererSettings m_dirtySettings;
    int m_numFrameDrawCalls = 0;

    // Constant Buffers (move somewhere?)
    ConstantBufferID    m_cameraConstantsGPU   = RendererUtils::InvalidID;
    ConstantBufferID    m_modelConstantsGPU    = RendererUtils::InvalidID;
    ConstantBufferID    m_fontConstantsGPU     = RendererUtils::InvalidID;

    // Defaults
    ShaderID            m_defaultShader        = RendererUtils::InvalidID;
    TextureID           m_defaultTexture       = RendererUtils::InvalidID;
    FontID              m_defaultFont          = RendererUtils::InvalidID;

    // GPU Objects
    std::unordered_map<FontID, Font*> m_fonts;
    std::unordered_map<ShaderID, Shader*> m_shaders;
    std::unordered_map<TextureID, Texture*> m_textures;
    std::unordered_map<SwapchainID, Swapchain*> m_swapchains;
    std::unordered_map<VertexBufferID, VertexBuffer*> m_vertexBuffers;
    std::unordered_map<RenderTargetID, RenderTarget*> m_renderTargets;
    std::unordered_map<ConstantBufferID, ConstantBuffer*> m_constantBuffers;

    // Mutexes
    mutable std::mutex m_fontsMutex;
    mutable std::mutex m_shadersMutex;
    mutable std::mutex m_texturesMutex;
    mutable std::mutex m_swapchainsMutex;
    mutable std::mutex m_vertexBuffersMutex;
    mutable std::mutex m_constantBuffersMutex;
    mutable std::mutex m_renderTargetsMutex;

    // Debug
#if defined(_DEBUG)
    bool m_debugDrawVertexBuffers = false;
    VertexBufferID m_debugVertexBuffer = RendererUtils::InvalidID;
#endif
};

