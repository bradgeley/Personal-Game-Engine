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
class GPUBuffer;
class Shader;
class Texture;
class Swapchain;
class VertexBuffer;
class InstanceBuffer;
class Window;
struct GpuBufferConfig;
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
    void DrawInstanced(VertexBuffer& vbo, InstanceBuffer& ibo);

    void SetMSAA(bool msaaEnabled);
    virtual void MSAAChanged() = 0;
    void SetVSync(bool vsyncEnabled);

    virtual void ClearDepth(float depth) = 0;

    // Factory Make Functions
    virtual TextureID           MakeTexture() = 0;
    virtual ShaderID            MakeShader(ShaderConfig const& config) = 0;
    virtual ConstantBufferID    MakeConstantBuffer(size_t initialSize) = 0;
    virtual VertexBufferID      MakeVertexBuffer() = 0;
    virtual InstanceBufferID    MakeInstanceBuffer() = 0;
    virtual SwapchainID         MakeSwapchain() = 0;
    virtual RenderTargetID      MakeSwapchainRenderTarget(void* hwnd, IntVec2 const& initialDims) = 0;
    virtual FontID              MakeFont();
	virtual GPUBuffer*          MakeGPUBuffer(GpuBufferConfig const& config) = 0;    // Does not own lifetime of this buffer

    template<typename VertexType>
    VertexBufferID MakeVertexBuffer(size_t numVerts = 0);

    template<typename InstanceType>
    InstanceBufferID MakeInstanceBuffer(size_t numInstances = 0);

    // Factory Get Functions
    Texture*                    GetTexture(TextureID id) const;
    Shader*                     GetShader(ShaderID id) const;
    ConstantBuffer*             GetConstantBuffer(ConstantBufferID id) const;
    VertexBuffer*               GetVertexBuffer(VertexBufferID id) const;
    InstanceBuffer*             GetInstanceBuffer(InstanceBufferID id) const;
    Swapchain*                  GetSwapchain(SwapchainID id) const;
    RenderTarget*               GetRenderTarget(RenderTargetID id);
    Font*                       GetFont(FontID id);

    // Factory Release Functions
    void                        ReleaseTexture(TextureID id);
    void                        ReleaseShader(ShaderID id);
    void                        ReleaseConstantBuffer(ConstantBufferID id);
    void                        ReleaseVertexBuffer(VertexBufferID id);
    void                        ReleaseInstanceBuffer(InstanceBufferID id);
    void                        ReleaseSwapchain(SwapchainID id);
    void                        ReleaseRenderTarget(RenderTargetID id);
    void                        ReleaseFont(FontID id);

    // Rendering Pipeline State
    void ResetRenderingPipelineState();
	void ResetCameraConstants();
	void ResetModelConstants();
	void ResetFontConstants();

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
    void BindTexture(Texture* texture, int slot = 0);
    void BindTexture(TextureID texture, int slot = 0);
    virtual void BindRenderTarget(RenderTargetID renderTarget) = 0;
    virtual void BindVertexBuffer(VertexBufferID vbo) const = 0;
    virtual void BindVertexBuffer(VertexBuffer& vbo) const = 0;
	virtual void BindInstanceBuffer(InstanceBufferID ibo) const = 0;
	virtual void BindInstanceBuffer(InstanceBuffer& ibo) const = 0;
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
    Texture* GetBoundTexture(int slot = 0) const;

protected:

    // ID Functions
    TextureID        RequestTextureID() const;
    ShaderID         RequestShaderID() const;
    ConstantBufferID RequestConstantBufferID() const;
    VertexBufferID   RequestVertexBufferID() const;
    InstanceBufferID RequestInstanceBufferID() const;
    SwapchainID      RequestSwapchainID() const;
    RenderTargetID   RequestRenderTargetID() const;
    FontID           RequestFontID() const;

    virtual VertexBufferID MakeTypedVertexBufferInternal(size_t vertSize, size_t numVerts);
    virtual InstanceBufferID MakeTypedInstanceBufferInternal(size_t instanceSize, size_t numInstances);

    virtual void Draw(int vertexCount, int vertexOffset = 0);
    virtual void DrawInstanced(int vertexCount, int instanceCount, int vertexOffset = 0, int instanceOffset = 0);

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
    virtual void BoundTextureUpdated(int slot = 0) = 0;
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
    virtual void DestroyInstanceBuffers();
    virtual void DestroySwapchains();
    virtual void DestroyRenderTargets();
    virtual void DestroyFonts();

    // Dev Console Commands
    void AddDevConsoleCommands();
    void RemoveDevConsoleCommands();
    bool DebugDrawVertexBuffers(NamedProperties& args);
    bool ToggleVSync(NamedProperties& args);
    virtual bool ToggleMSAA(NamedProperties& args);

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
    std::unordered_map<InstanceBufferID, InstanceBuffer*> m_instanceBuffers;
    std::unordered_map<ConstantBufferID, ConstantBuffer*> m_constantBuffers;

    // Mutexes
    mutable std::mutex m_fontsMutex;
    mutable std::mutex m_shadersMutex;
    mutable std::mutex m_texturesMutex;
    mutable std::mutex m_swapchainsMutex;
    mutable std::mutex m_vertexBuffersMutex;
    mutable std::mutex m_constantBuffersMutex;
    mutable std::mutex m_instanceBuffersMutex;
    mutable std::mutex m_renderTargetsMutex;

    // Debug
#if defined(_DEBUG)
    bool m_debugDrawVertexBuffers = false;
    VertexBufferID m_debugVertexBuffer = RendererUtils::InvalidID;
#endif
};



//----------------------------------------------------------------------------------------------------------------------
template<typename VertexType>
inline VertexBufferID Renderer::MakeVertexBuffer(size_t numVerts)
{
    return MakeTypedVertexBufferInternal(sizeof(VertexType), numVerts);
}



//----------------------------------------------------------------------------------------------------------------------
template<typename InstanceType>
inline InstanceBufferID Renderer::MakeInstanceBuffer(size_t numInstances)
{
    return MakeTypedInstanceBufferInternal(sizeof(InstanceType), numInstances);
}
