// Bradley Christensen - 2022-2025
#include "Renderer.h"
#include "Engine/Core/ErrorUtils.h"
#include "Engine/Debug/DebugDrawUtils.h"
#include "Engine/Debug/DevConsole.h"
#include "Engine/Events/EventSystem.h"
#include "Camera.h"
#include "ConstantBuffer.h"
#include "Font.h"
#include "RenderTarget.h"
#include "Shader.h"
#include "Swapchain.h"
#include "Texture.h"
#include "VertexBuffer.h"
#include "VertexUtils.h"
#include "Window.h"
#include <thread>



//----------------------------------------------------------------------------------------------------------------------
// THE Renderer
//
Renderer* g_renderer = nullptr;



//----------------------------------------------------------------------------------------------------------------------
Renderer::Renderer(RendererConfig const& config) : EngineSubsystem("Renderer"), m_config(config)
{
    
}



//----------------------------------------------------------------------------------------------------------------------
void Renderer::Startup()
{
	CreateDebugLayer();
	CreateDevice();
	CreateDefaultShader();
	CreateDefaultTexture();
	CreateBlendStates();
	CreateRasterizerState();
	CreateConstantBuffers();
	CreateDefaultFont();

	UpdateRenderingPipelineState(true);
    AddDevConsoleCommands();

	#if defined(_DEBUG)
		m_debugVertexBuffer = MakeVertexBuffer();
	#endif
}



//----------------------------------------------------------------------------------------------------------------------
void Renderer::Render() const
{

}



//----------------------------------------------------------------------------------------------------------------------
void Renderer::Shutdown()
{
	DestroyShaders();
	DestroyTextures();
	DestroyConstantBuffers();
	DestroyVertexBuffers();
	DestroySwapchains();
	DestroyRenderTargets();
	DestroyFonts();

	DestroyBlendStates();
	DestroyRasterizerState();
	DestroySamplerStates();
	DestroyDepthStencilState();
	DestroyDevice();

	DestroyDebugLayer();

	RemoveDevConsoleCommands();
}



//----------------------------------------------------------------------------------------------------------------------
void Renderer::BeginFrame()
{
	m_numFrameDrawCalls = 0;
}



//----------------------------------------------------------------------------------------------------------------------
void Renderer::EndFrame()
{
	if (m_currentCamera)
	{
		EndCamera(m_currentCamera);
	}

	Present();

	if (m_currentRenderTarget != RendererUtils::InvalidID)
	{
		UnbindRenderTarget(m_currentRenderTarget);
	}

	std::this_thread::sleep_for(std::chrono::milliseconds(1));
}



//----------------------------------------------------------------------------------------------------------------------
void Renderer::BeginWindow(Window const* window)
{
	ASSERT_OR_DIE(window, "Renderer::BeginWindow - trying to begin a null window.");

	BindRenderTarget(window->GetRenderTarget());
}



//----------------------------------------------------------------------------------------------------------------------
void Renderer::EndWindow(Window const* window)
{
	ASSERT_OR_DIE(window, "Renderer::EndWindow - trying to end a null window.");
	ASSERT_OR_DIE(window->GetRenderTarget() == m_currentRenderTarget, "Renderer::EndWindow - window does not match m_currentRenderTarget");

	UnbindRenderTarget(m_currentRenderTarget);
}



//----------------------------------------------------------------------------------------------------------------------
void Renderer::BeginCamera(Camera const* camera)
{
	ASSERT_OR_DIE(camera, "Renderer::BeginCamera - trying to begin a null camera.");

	m_currentCamera = camera;

	ResetRenderingPipelineState();

	// Fill Camera Constants
	m_dirtySettings.m_cameraConstants = camera->GetCameraConstants();
}



//----------------------------------------------------------------------------------------------------------------------
void Renderer::EndCamera(Camera const* camera)
{
	ASSERT_OR_DIE(camera, "Renderer::EndCamera - trying to end null camera.");
	ASSERT_OR_DIE(camera == m_currentCamera, "Renderer::EndCamera - camera is not equal to m_currentCamera.");

	m_currentCamera = nullptr;
}



//----------------------------------------------------------------------------------------------------------------------
Camera const* Renderer::GetCurrentCamera() const
{
	return m_currentCamera;
}



//----------------------------------------------------------------------------------------------------------------------
void Renderer::BeginCameraAndWindow(Camera const* camera, Window const* window)
{
	BeginWindow(window);
	BeginCamera(camera);
}



//----------------------------------------------------------------------------------------------------------------------
void Renderer::EndCameraAndWindow(Camera const* camera, Window const* window)
{
	EndCamera(camera);
	EndWindow(window);
}



//----------------------------------------------------------------------------------------------------------------------
RendererUserSettings Renderer::GetPerUserSettings() const
{
    return m_perUserSettings;
}



//----------------------------------------------------------------------------------------------------------------------
void Renderer::DrawVertexBuffer(VertexBufferID id)
{
	VertexBuffer* vbo = GetVertexBuffer(id);
	ASSERT_OR_DIE(vbo, "Trying to draw invalid vbo.");

	DrawVertexBuffer(*vbo);
}



//----------------------------------------------------------------------------------------------------------------------
void Renderer::DrawVertexBuffer(VertexBuffer& vbo)
{
	if (vbo.IsDirty())
	{
		vbo.UpdateGPUBuffer();
	}
	if (vbo.GetNumVerts() > 0)
	{
		// Only bother drawing it if there are verts to draw
		BindVertexBuffer(vbo);
		Draw(vbo.GetNumVerts(), 0);
	}

	#if defined(_DEBUG)
		if (m_debugDrawVertexBuffers)
		{
			float thickness = m_currentCamera->GetOrthoDimensions().GetLowestXY() / 1000.f;

			VertexBuffer& debugVBO = *GetVertexBuffer(m_debugVertexBuffer);
			AddVertsForWireMesh2D(debugVBO, vbo, thickness, Rgba8::Magenta);
			BindShader(nullptr);
			BindTexture(nullptr);

			m_debugDrawVertexBuffers = false; // avoid infinite recursion
			DrawVertexBuffer(debugVBO);
			m_debugDrawVertexBuffers = true;

			debugVBO.ClearVerts();
		}
	#endif
}



//----------------------------------------------------------------------------------------------------------------------
Texture* Renderer::GetTexture(TextureID id) const
{
	std::unique_lock lock(m_texturesMutex);
	auto it = m_textures.find(id);
	if (it != m_textures.end())
	{
		return it->second;
	}
	return nullptr;
}



//----------------------------------------------------------------------------------------------------------------------
Shader* Renderer::GetShader(ShaderID id) const
{
	std::unique_lock lock(m_shadersMutex);
	auto it = m_shaders.find(id);
	if (it != m_shaders.end())
	{
		return it->second;
	}
	return nullptr;
}



//----------------------------------------------------------------------------------------------------------------------
ConstantBuffer* Renderer::GetConstantBuffer(ConstantBufferID id) const
{
	std::unique_lock lock(m_constantBuffersMutex);
	auto it = m_constantBuffers.find(id);
	if (it != m_constantBuffers.end())
	{
		return it->second;
	}
	return nullptr;
}



//----------------------------------------------------------------------------------------------------------------------
VertexBuffer* Renderer::GetVertexBuffer(VertexBufferID id) const
{
	std::unique_lock lock(m_vertexBuffersMutex);
	auto it = m_vertexBuffers.find(id);
	if (it != m_vertexBuffers.end())
	{
		return it->second;
	}
	return nullptr;
}



//----------------------------------------------------------------------------------------------------------------------
Swapchain* Renderer::GetSwapchain(SwapchainID id) const
{
	std::unique_lock lock(m_swapchainsMutex);
	auto it = m_swapchains.find(id);
	if (it != m_swapchains.end())
	{
		return it->second;
	}
	return nullptr;
}



//----------------------------------------------------------------------------------------------------------------------
RenderTarget* Renderer::GetRenderTarget(RenderTargetID id)
{
	std::unique_lock lock(m_renderTargetsMutex);
	auto it = m_renderTargets.find(id);
	if (it != m_renderTargets.end())
	{
		return it->second;
	}
	return nullptr;
}



//----------------------------------------------------------------------------------------------------------------------
Font* Renderer::GetFont(FontID id)
{
	std::unique_lock lock(m_fontsMutex);
	auto it = m_fonts.find(id);
	if (it != m_fonts.end())
	{
		return it->second;
	}
	return nullptr;
}



//----------------------------------------------------------------------------------------------------------------------
void Renderer::ReleaseTexture(TextureID id)
{
	std::unique_lock lock(m_texturesMutex);
	auto pair = m_textures.find(id);
	if (pair != m_textures.end())
	{
		Texture* texture = pair->second;
		texture->ReleaseResources();
		delete texture;
		m_textures.erase(pair);
	}
}



//----------------------------------------------------------------------------------------------------------------------
void Renderer::ReleaseShader(ShaderID id)
{
	std::unique_lock lock(m_shadersMutex);
	auto pair = m_shaders.find(id);
	if (pair != m_shaders.end())
	{
		Shader* shader = pair->second;
		shader->ReleaseResources();
		delete shader;
		m_shaders.erase(pair);
	}
}



//----------------------------------------------------------------------------------------------------------------------
void Renderer::ReleaseConstantBuffer(ConstantBufferID id)
{
	std::unique_lock lock(m_constantBuffersMutex);
	auto pair = m_constantBuffers.find(id);
	if (pair != m_constantBuffers.end())
	{
		ConstantBuffer* cb = pair->second;
		cb->ReleaseResources();
		delete cb;
		m_constantBuffers.erase(pair);
	}
}



//----------------------------------------------------------------------------------------------------------------------
void Renderer::ReleaseVertexBuffer(VertexBufferID id)
{
	std::unique_lock lock(m_vertexBuffersMutex);
	auto pair = m_vertexBuffers.find(id);
	if (pair != m_vertexBuffers.end())
	{
		VertexBuffer* vb = pair->second;
		vb->ReleaseResources();
		delete vb;
		m_vertexBuffers.erase(pair);
	}
}



//----------------------------------------------------------------------------------------------------------------------
void Renderer::ReleaseSwapchain(SwapchainID id)
{
	std::unique_lock lock(m_swapchainsMutex);
	auto pair = m_swapchains.find(id);
	if (pair != m_swapchains.end())
	{
		Swapchain* sc = pair->second;
		sc->ReleaseResources();
		delete sc;
		m_swapchains.erase(pair);
	}
}




//----------------------------------------------------------------------------------------------------------------------
void Renderer::ReleaseRenderTarget(RenderTargetID renderTargetID)
{
	std::unique_lock lock(m_renderTargetsMutex);
	auto pair = m_renderTargets.find(renderTargetID);
	if (pair != m_renderTargets.end())
	{
		RenderTarget* rt = pair->second;
		rt->ReleaseResources();
		delete rt;
		m_renderTargets.erase(pair);
	}
}



//----------------------------------------------------------------------------------------------------------------------
void Renderer::ReleaseFont(FontID id)
{
	std::unique_lock lock(m_fontsMutex);
	auto pair = m_fonts.find(id);
	if (pair != m_fonts.end())
	{
		Font* font = pair->second;
		font->ReleaseResources();
		delete font;
		m_fonts.erase(pair);
	}
}



//----------------------------------------------------------------------------------------------------------------------
void Renderer::ResetRenderingPipelineState()
{
	// Only reset the dirty settings, so changes will be detected and applied on Draw
	m_dirtySettings = RendererSettings();
	BindShader(m_defaultShader);
	BindTexture(m_defaultTexture);
}



//----------------------------------------------------------------------------------------------------------------------
void Renderer::SetCameraConstants(CameraConstants const& cameraConstants)
{
	m_dirtySettings.m_cameraConstants = cameraConstants;
}



//----------------------------------------------------------------------------------------------------------------------
void Renderer::SetModelConstants(ModelConstants const& modelConstants)
{
	m_dirtySettings.m_modelConstants = modelConstants;
}



//----------------------------------------------------------------------------------------------------------------------
void Renderer::SetFontConstants(FontConstants const& fontConstants)
{
	m_dirtySettings.m_fontConstants = fontConstants;
}



//----------------------------------------------------------------------------------------------------------------------
void Renderer::SetModelMatrix(Mat44 const& modelMatrix)
{
	m_dirtySettings.m_modelConstants.m_modelMatrix = modelMatrix;
}



//----------------------------------------------------------------------------------------------------------------------
void Renderer::SetModelTint(Rgba8 const& modelTint)
{
	modelTint.GetAsFloats(m_dirtySettings.m_modelConstants.m_modelRgba);
}



//----------------------------------------------------------------------------------------------------------------------
void Renderer::SetBlendMode(BlendMode blendMode)
{
	m_dirtySettings.m_blendMode = blendMode;
}



//----------------------------------------------------------------------------------------------------------------------
void Renderer::SetSamplerMode(SamplerFilter filter, SamplerAddressMode addressMode)
{
	m_dirtySettings.m_samplerFilter = filter;
	m_dirtySettings.m_samplerAddressMode = addressMode;
}



//----------------------------------------------------------------------------------------------------------------------
void Renderer::SetCullMode(CullMode cullMode)
{
	m_dirtySettings.m_cullMode = cullMode;
}



//----------------------------------------------------------------------------------------------------------------------
void Renderer::SetWindingOrder(Winding winding)
{
	m_dirtySettings.m_winding = winding;
}



//----------------------------------------------------------------------------------------------------------------------
void Renderer::SetFillMode(FillMode fillMode)
{
	m_dirtySettings.m_fillMode = fillMode;
}



//----------------------------------------------------------------------------------------------------------------------
void Renderer::BindTexture(TextureID texture)
{
	m_dirtySettings.m_boundTexture = texture;
}



//----------------------------------------------------------------------------------------------------------------------
void Renderer::BindShader(Shader* shader)
{
	if (!shader)
	{
		m_dirtySettings.m_boundShader = m_defaultShader;
		return;
	}

	for (auto it : m_shaders)
	{
		if (it.second == shader)
		{
			m_dirtySettings.m_boundShader = it.first;
			return;
		}
	}
}



//----------------------------------------------------------------------------------------------------------------------
void Renderer::BindShader(ShaderID shader)
{
	m_dirtySettings.m_boundShader = shader;
}



//----------------------------------------------------------------------------------------------------------------------
void Renderer::BindTexture(Texture* texture)
{
	if (!texture)
	{
		m_dirtySettings.m_boundTexture = m_defaultTexture;
		return;
	}

	for (auto it : m_textures)
	{
		if (it.second == texture)
		{
			m_dirtySettings.m_boundTexture = it.first;
			return;
		}
	}
}



//----------------------------------------------------------------------------------------------------------------------
void Renderer::UnbindRenderTarget(RenderTargetID)
{
	m_currentRenderTarget = RendererUtils::InvalidID;
}



//----------------------------------------------------------------------------------------------------------------------
int Renderer::GetNumFrameDrawCalls() const
{
	return m_numFrameDrawCalls;
}



//----------------------------------------------------------------------------------------------------------------------
Font* Renderer::GetDefaultFont() const
{
	auto it = m_fonts.find(m_defaultFont);
	ASSERT_OR_DIE(it != m_fonts.end(), "Cannot find default font.");
	return it->second;
}



//----------------------------------------------------------------------------------------------------------------------
Texture* Renderer::GetDefaultTexture() const
{
	auto it = m_textures.find(m_defaultTexture);
	ASSERT_OR_DIE(it != m_textures.end(), "Cannot find default texture.");
	return it->second;
}



//----------------------------------------------------------------------------------------------------------------------
Shader* Renderer::GetDefaultShader() const
{
	auto it = m_shaders.find(m_defaultShader);
	ASSERT_OR_DIE(it != m_shaders.end(), "Cannot find default shader.");
	return it->second;
}



//----------------------------------------------------------------------------------------------------------------------
Shader* Renderer::GetBoundShader() const
{
	return GetShader(m_settings.m_boundShader);
}



//----------------------------------------------------------------------------------------------------------------------
Texture* Renderer::GetBoundTexture() const
{
	return GetTexture(m_settings.m_boundTexture);
}



//----------------------------------------------------------------------------------------------------------------------
TextureID Renderer::RequestTextureID() const
{
	static TextureID s_id = 0;
	return s_id++;
}



//----------------------------------------------------------------------------------------------------------------------
ShaderID Renderer::RequestShaderID() const
{
	static ShaderID s_id = 0;
	return s_id++;
}



//----------------------------------------------------------------------------------------------------------------------
ConstantBufferID Renderer::RequestConstantBufferID() const
{
	static ConstantBufferID s_id = 0;
	return s_id++;
}



//----------------------------------------------------------------------------------------------------------------------
VertexBufferID Renderer::RequestVertexBufferID() const
{
	static VertexBufferID s_id = 0;
	return s_id++;
}



//----------------------------------------------------------------------------------------------------------------------
SwapchainID Renderer::RequestSwapchainID() const
{
	static SwapchainID s_id = 0;
	return s_id++;
}



//----------------------------------------------------------------------------------------------------------------------
RenderTargetID Renderer::RequestRenderTargetID() const
{
	static RenderTargetID s_id = 0;
	return s_id++;
}



//----------------------------------------------------------------------------------------------------------------------
FontID Renderer::RequestFontID() const
{
	static FontID s_id = 0;
	return s_id++;
}



//----------------------------------------------------------------------------------------------------------------------
void Renderer::Draw(int, int)
{
	UpdateRenderingPipelineState();
	m_numFrameDrawCalls++;
}



//----------------------------------------------------------------------------------------------------------------------
void Renderer::UpdateRenderingPipelineState(bool force /*= false*/)
{
	UpdateRasterizerState(force);
	UpdateDepthStencilState(force);
	UpdateModelConstants(force);
	UpdateCameraConstants(force);
	UpdateFontConstants(force);
	UpdateBlendMode(force);
	UpdateSamplerState(force);
	UpdateTexture(force);
	UpdateShader(force);
}



//----------------------------------------------------------------------------------------------------------------------
void Renderer::UpdateRasterizerState(bool force)
{
	if (force ||
		m_dirtySettings.m_cullMode != m_settings.m_cullMode ||
		m_dirtySettings.m_winding != m_settings.m_winding ||
		m_dirtySettings.m_fillMode != m_settings.m_fillMode)
	{
		m_settings.m_cullMode = m_dirtySettings.m_cullMode;
		m_settings.m_winding = m_dirtySettings.m_winding;
		m_settings.m_fillMode = m_dirtySettings.m_fillMode;

		RasterizerStateUpdated();
	}
}



//----------------------------------------------------------------------------------------------------------------------
void Renderer::UpdateDepthStencilState(bool force)
{
	if (force ||
		m_dirtySettings.m_writeDepth != m_settings.m_writeDepth ||
		m_dirtySettings.m_depthTest != m_settings.m_depthTest)
	{
		m_settings.m_writeDepth = m_dirtySettings.m_writeDepth;
		m_settings.m_depthTest = m_dirtySettings.m_depthTest;

		DepthStencilStateUpdated();
	}
}



//----------------------------------------------------------------------------------------------------------------------
void Renderer::UpdateModelConstants(bool force)
{
	if (force || m_dirtySettings.m_modelConstants != m_settings.m_modelConstants)
	{
		m_settings.m_modelConstants = m_dirtySettings.m_modelConstants;

		ModelConstantsUpdated();
	}
}



//----------------------------------------------------------------------------------------------------------------------
void Renderer::UpdateCameraConstants(bool force)
{
	if (force || m_dirtySettings.m_cameraConstants != m_settings.m_cameraConstants)
	{
		m_settings.m_cameraConstants = m_dirtySettings.m_cameraConstants;

		CameraConstantsUpdated();
	}
}



//----------------------------------------------------------------------------------------------------------------------
void Renderer::UpdateFontConstants(bool force)
{
	if (force || m_dirtySettings.m_fontConstants != m_settings.m_fontConstants)
	{
		m_settings.m_fontConstants = m_dirtySettings.m_fontConstants;

		FontConstantsUpdated();
	}
}



//----------------------------------------------------------------------------------------------------------------------
void Renderer::UpdateBlendMode(bool force)
{
	if (force || m_dirtySettings.m_blendMode != m_settings.m_blendMode)
	{
		m_settings.m_blendMode = m_dirtySettings.m_blendMode;

		BlendModeUpdated();
	}
}



//----------------------------------------------------------------------------------------------------------------------
void Renderer::UpdateSamplerState(bool force)
{
	if (force ||
		m_dirtySettings.m_samplerFilter != m_settings.m_samplerFilter ||
		m_dirtySettings.m_samplerAddressMode != m_settings.m_samplerAddressMode)
	{
		m_settings.m_samplerFilter = m_dirtySettings.m_samplerFilter;
		m_settings.m_samplerAddressMode = m_dirtySettings.m_samplerAddressMode;

		SamplerStateUpdated();
	}
}



//----------------------------------------------------------------------------------------------------------------------
void Renderer::UpdateTexture(bool force)
{
	if (force || m_dirtySettings.m_boundTexture != m_settings.m_boundTexture)
	{
		if (m_dirtySettings.m_boundTexture == RendererUtils::InvalidID)
		{
			ASSERT_OR_DIE(m_defaultTexture != RendererUtils::InvalidID, "No default texture available.");
			BindTexture(m_defaultTexture);
		}
		m_settings.m_boundTexture = m_dirtySettings.m_boundTexture;

		BoundTextureUpdated();
	}
}



//----------------------------------------------------------------------------------------------------------------------
void Renderer::UpdateShader(bool force)
{
	if (force || m_dirtySettings.m_boundShader != m_settings.m_boundShader)
	{
		if (m_dirtySettings.m_boundShader == RendererUtils::InvalidID)
		{
			ASSERT_OR_DIE(m_defaultShader != RendererUtils::InvalidID, "No default shader exists.");
			BindShader(m_defaultShader);
		}
		m_settings.m_boundShader = m_dirtySettings.m_boundShader;

		BoundShaderUpdated();
	}
}



//----------------------------------------------------------------------------------------------------------------------
void Renderer::CreateConstantBuffers()
{
	m_cameraConstantsGPU = MakeConstantBuffer();
	m_modelConstantsGPU = MakeConstantBuffer();
	m_fontConstantsGPU = MakeConstantBuffer();

	GetConstantBuffer(m_cameraConstantsGPU)->Initialize(sizeof(CameraConstants));
	GetConstantBuffer(m_modelConstantsGPU)->Initialize(sizeof(ModelConstants));
	GetConstantBuffer(m_fontConstantsGPU)->Initialize(sizeof(FontConstants));

	BindConstantBuffer(m_cameraConstantsGPU, 2);
	BindConstantBuffer(m_modelConstantsGPU, 3);
	BindConstantBuffer(m_fontConstantsGPU, 4);
}



//----------------------------------------------------------------------------------------------------------------------
void Renderer::DestroyShaders()
{
	for (auto shader : m_shaders)
	{
		shader.second->ReleaseResources();
		delete shader.second;
	}
	m_shaders.clear();
}



//----------------------------------------------------------------------------------------------------------------------
void Renderer::DestroyTextures()
{
	for (auto tex : m_textures)
	{
		tex.second->ReleaseResources();
		delete tex.second;
	}
	m_textures.clear();
}



//----------------------------------------------------------------------------------------------------------------------
void Renderer::DestroyConstantBuffers()
{
	for (auto cbo : m_constantBuffers)
	{
		cbo.second->ReleaseResources();
		delete cbo.second;
	}
	m_constantBuffers.clear();
}



//----------------------------------------------------------------------------------------------------------------------
void Renderer::DestroyVertexBuffers()
{
	for (auto vb : m_vertexBuffers)
	{
		vb.second->ReleaseResources();
		delete vb.second;
	}
	m_vertexBuffers.clear();


	#if defined(_DEBUG)
		m_debugVertexBuffer = RendererUtils::InvalidID;
	#endif
}



//----------------------------------------------------------------------------------------------------------------------
void Renderer::DestroySwapchains()
{
	for (auto sc : m_swapchains)
	{
		sc.second->ReleaseResources();
		delete sc.second;
	}
	m_swapchains.clear();
}



//----------------------------------------------------------------------------------------------------------------------
void Renderer::DestroyRenderTargets()
{
	for (auto rt : m_renderTargets)
	{
		rt.second->ReleaseResources();
		delete rt.second;
	}
	m_renderTargets.clear();
}



//----------------------------------------------------------------------------------------------------------------------
void Renderer::DestroyFonts()
{
	for (auto font : m_fonts)
	{
		font.second->ReleaseResources();
		delete font.second;
	}
	m_fonts.clear();
}



//----------------------------------------------------------------------------------------------------------------------
void Renderer::ModelConstantsUpdated()
{
	ASSERT_OR_DIE(m_modelConstantsGPU != RendererUtils::InvalidID, "Updating invalid constant buffer.");
	GetConstantBuffer(m_modelConstantsGPU)->Update(&m_settings.m_modelConstants, sizeof(ModelConstants));
}



//----------------------------------------------------------------------------------------------------------------------
void Renderer::CameraConstantsUpdated()
{
	ASSERT_OR_DIE(m_cameraConstantsGPU != RendererUtils::InvalidID, "Updating invalid constant buffer.");
	GetConstantBuffer(m_cameraConstantsGPU)->Update(&m_settings.m_cameraConstants, sizeof(CameraConstants));
}



//----------------------------------------------------------------------------------------------------------------------
void Renderer::FontConstantsUpdated()
{
	ASSERT_OR_DIE(m_fontConstantsGPU != RendererUtils::InvalidID, "Updating invalid constant buffer.");
	GetConstantBuffer(m_fontConstantsGPU)->Update(&m_settings.m_fontConstants, sizeof(FontConstants));
}



//----------------------------------------------------------------------------------------------------------------------
void Renderer::AddDevConsoleCommands()
{
	if (g_eventSystem)
	{
		g_eventSystem->SubscribeMethod("DebugDrawVertexBuffers", this, &Renderer::DebugDrawVertexBuffers);
		g_eventSystem->SubscribeMethod("ToggleVSync", this, &Renderer::ToggleVSync);
		g_eventSystem->SubscribeMethod("ToggleMSAA", this, &Renderer::ToggleMSAA);
	}
}



//----------------------------------------------------------------------------------------------------------------------
void Renderer::RemoveDevConsoleCommands()
{
	if (g_eventSystem)
	{
		g_eventSystem->UnsubscribeMethod("DebugDrawVertexBuffers", this, &Renderer::DebugDrawVertexBuffers);
		g_eventSystem->UnsubscribeMethod("ToggleVSync", this, &Renderer::ToggleVSync);
		g_eventSystem->UnsubscribeMethod("ToggleMSAA", this, &Renderer::ToggleMSAA);
	}
}



//----------------------------------------------------------------------------------------------------------------------
bool Renderer::DebugDrawVertexBuffers(NamedProperties&)
{
	#if defined(_DEBUG)
		m_debugDrawVertexBuffers = !m_debugDrawVertexBuffers;
		return true;
	#else
		g_devConsole->LogError("Cannot debug draw vertex buffers in a Release build.");
		return false;
	#endif
}



//----------------------------------------------------------------------------------------------------------------------
bool Renderer::ToggleVSync(NamedProperties&)
{
	m_perUserSettings.m_vsyncEnabled = !m_perUserSettings.m_vsyncEnabled;
	return false;
}



//----------------------------------------------------------------------------------------------------------------------
// TODO: Move factory function to its own file so the renderer interface doesnt have to have them all
//
#include "D3D11/D3D11Renderer.h"



//----------------------------------------------------------------------------------------------------------------------
Renderer* Renderer::MakeRenderer(RendererConfig const& config)
{
	return new D3D11Renderer(config);
}



//----------------------------------------------------------------------------------------------------------------------
FontID Renderer::MakeFont()
{
	auto fontResult = new Font();
	std::unique_lock lock(m_fontsMutex);
	FontID fontID = RequestFontID();
	m_fonts[fontID] = fontResult;
	return fontID;
}