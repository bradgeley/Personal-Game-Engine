// Bradley Christensen - 2022-2023
#include "RendererInterface.h"
#include "Engine/Core/ErrorUtils.h"
#include "Engine/Debug/DebugDrawUtils.h"
#include "Engine/Events/EventSystem.h"
#include "Camera.h"
#include "ConstantBuffer.h"
#include "Font.h"
#include "RenderTarget.h"
#include "Shader.h"
#include "Texture.h"
#include "VertexBuffer.h"
#include "VertexUtils.h"
#include "Window.h"
#include <thread>



//----------------------------------------------------------------------------------------------------------------------
// THE Renderer
//
RendererInterface* g_rendererInterface = nullptr;



//----------------------------------------------------------------------------------------------------------------------
uint32_t INVALID_RENDER_TARGET_ID = UINT32_MAX;



//----------------------------------------------------------------------------------------------------------------------
RendererInterface::RendererInterface(RendererConfig const& config) : EngineSubsystem("Renderer"), m_config(config)
{
    
}



//----------------------------------------------------------------------------------------------------------------------
void RendererInterface::Startup()
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
}



//----------------------------------------------------------------------------------------------------------------------
void RendererInterface::Render() const
{

}



//----------------------------------------------------------------------------------------------------------------------
void RendererInterface::Shutdown()
{
	DestroyDefaultFont();
	DestroyDefaultShader();
	DestroyDefaultTexture();
	DestroyBlendStates();
	DestroyRasterizerState();
	DestroyConstantBuffers();
	DestroySamplerStates();
	DestroyDepthStencilState();
	DestroyDevice();

	DestroyDebugLayer();

	RemoveDevConsoleCommands();
}



//----------------------------------------------------------------------------------------------------------------------
void RendererInterface::BeginFrame()
{
	m_numFrameDrawCalls = 0;
}



//----------------------------------------------------------------------------------------------------------------------
void RendererInterface::EndFrame()
{
	if (m_currentCamera)
	{
		EndCamera(m_currentCamera);
	}

	Present();

	if (m_currentRenderTarget != INVALID_RENDER_TARGET_ID)
	{
		UnbindRenderTarget(m_currentRenderTarget);
	}

	std::this_thread::sleep_for(std::chrono::milliseconds(1));
}



//----------------------------------------------------------------------------------------------------------------------
void RendererInterface::BeginWindow(Window const* window)
{
	ASSERT_OR_DIE(window, "Renderer::BeginWindow - trying to begin a null window.");

	BindRenderTarget(window->GetRenderTarget());
}



//----------------------------------------------------------------------------------------------------------------------
void RendererInterface::EndWindow(Window const* window)
{
	ASSERT_OR_DIE(window, "Renderer::EndWindow - trying to end a null window.");
	ASSERT_OR_DIE(window->GetRenderTarget() == m_currentRenderTarget, "Renderer::EndWindow - window does not match m_currentRenderTarget");

	UnbindRenderTarget(m_currentRenderTarget);
}



//----------------------------------------------------------------------------------------------------------------------
void RendererInterface::BeginCamera(Camera const* camera)
{
	ASSERT_OR_DIE(camera, "Renderer::BeginCamera - trying to begin a null camera.");

	m_currentCamera = camera;

	ResetRenderingPipelineState();

	// Fill Camera Constants
	m_dirtySettings.m_cameraConstants = camera->GetCameraConstants();
}



//----------------------------------------------------------------------------------------------------------------------
void RendererInterface::EndCamera(Camera const* camera)
{
	ASSERT_OR_DIE(camera, "Renderer::EndCamera - trying to end null camera.");
	ASSERT_OR_DIE(camera == m_currentCamera, "Renderer::EndCamera - camera is not equal to m_currentCamera.");

	m_currentCamera = nullptr;
}



//----------------------------------------------------------------------------------------------------------------------
Camera const* RendererInterface::GetCurrentCamera() const
{
	return m_currentCamera;
}



//----------------------------------------------------------------------------------------------------------------------
void RendererInterface::BeginCameraAndWindow(Camera const* camera, Window const* window)
{
	BeginWindow(window);
	BeginCamera(camera);
}



//----------------------------------------------------------------------------------------------------------------------
void RendererInterface::EndCameraAndWindow(Camera const* camera, Window const* window)
{
	EndCamera(camera);
	EndWindow(window);
}



//----------------------------------------------------------------------------------------------------------------------
RendererPerUserSettings RendererInterface::GetPerUserSettings() const
{
    return m_perUserSettings;
}



//----------------------------------------------------------------------------------------------------------------------
void RendererInterface::DrawVertexBuffer(VertexBuffer* vbo)
{
	if (vbo->IsDirty())
	{
		vbo->UpdateGPUBuffer();
	}
	if (vbo->GetNumVerts() > 0)
	{
		// Only bother drawing it if there are verts to draw
		BindVertexBuffer(vbo);
		Draw(vbo->GetNumVerts(), 0);
	}

	#if defined(_DEBUG)
	if (m_debugDrawVertexBuffers)
	{
		float thickness = m_currentCamera->GetOrthoDimensions().GetLowestXY() / 1000.f;

		m_debugDrawVertexBuffers = false; // avoid infinite recursion
		DebugDrawMesh2D(vbo->GetVerts(), thickness);
		m_debugDrawVertexBuffers = true;
	}
	#endif
}



//----------------------------------------------------------------------------------------------------------------------
void RendererInterface::ResetRenderingPipelineState()
{
	// Only reset the dirty settings, so changes will be detected and applied on Draw
	m_dirtySettings = RendererSettings();
	BindShader(m_defaultShader);
	BindTexture(m_defaultTexture);
}



//----------------------------------------------------------------------------------------------------------------------
void RendererInterface::SetCameraConstants(CameraConstants const& cameraConstants)
{
	m_dirtySettings.m_cameraConstants = cameraConstants;
}



//----------------------------------------------------------------------------------------------------------------------
void RendererInterface::SetModelConstants(ModelConstants const& modelConstants)
{
	m_dirtySettings.m_modelConstants = modelConstants;
}



//----------------------------------------------------------------------------------------------------------------------
void RendererInterface::SetFontConstants(FontConstants const& fontConstants)
{
	m_dirtySettings.m_fontConstants = fontConstants;
}



//----------------------------------------------------------------------------------------------------------------------
void RendererInterface::SetModelMatrix(Mat44 const& modelMatrix)
{
	m_dirtySettings.m_modelConstants.m_modelMatrix = modelMatrix;
}



//----------------------------------------------------------------------------------------------------------------------
void RendererInterface::SetModelTint(Rgba8 const& modelTint)
{
	modelTint.GetAsFloats(m_dirtySettings.m_modelConstants.m_modelRgba);
}



//----------------------------------------------------------------------------------------------------------------------
void RendererInterface::SetBlendMode(BlendMode blendMode)
{
	m_dirtySettings.m_blendMode = blendMode;
}



//----------------------------------------------------------------------------------------------------------------------
void RendererInterface::SetSamplerMode(SamplerFilter filter, SamplerAddressMode addressMode)
{
	m_dirtySettings.m_samplerFilter = filter;
	m_dirtySettings.m_samplerAddressMode = addressMode;
}



//----------------------------------------------------------------------------------------------------------------------
void RendererInterface::SetCullMode(CullMode cullMode)
{
	m_dirtySettings.m_cullMode = cullMode;
}



//----------------------------------------------------------------------------------------------------------------------
void RendererInterface::SetWindingOrder(Winding winding)
{
	m_dirtySettings.m_winding = winding;
}



//----------------------------------------------------------------------------------------------------------------------
void RendererInterface::SetFillMode(FillMode fillMode)
{
	m_dirtySettings.m_fillMode = fillMode;
}



//----------------------------------------------------------------------------------------------------------------------
void RendererInterface::BindTexture(Texture* texture)
{
	m_dirtySettings.m_texture = texture;
}



//----------------------------------------------------------------------------------------------------------------------
void RendererInterface::BindShader(Shader* shader)
{
	m_dirtySettings.m_shader = shader;
}



//----------------------------------------------------------------------------------------------------------------------
void RendererInterface::UnbindRenderTarget(RenderTargetID)
{
	m_currentRenderTarget = INVALID_RENDER_TARGET_ID;
}



//----------------------------------------------------------------------------------------------------------------------
int RendererInterface::GetNumFrameDrawCalls() const
{
	return m_numFrameDrawCalls;
}



//----------------------------------------------------------------------------------------------------------------------
Font* RendererInterface::GetDefaultFont() const
{
	return m_defaultFont;
}



//----------------------------------------------------------------------------------------------------------------------
void RendererInterface::Draw(int, int)
{
	UpdateRenderingPipelineState();
	m_numFrameDrawCalls++;
}



//----------------------------------------------------------------------------------------------------------------------
void RendererInterface::UpdateRenderingPipelineState(bool force /*= false*/)
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
void RendererInterface::UpdateRasterizerState(bool force)
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
void RendererInterface::UpdateDepthStencilState(bool force)
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
void RendererInterface::UpdateModelConstants(bool force)
{
	if (force || m_dirtySettings.m_modelConstants != m_settings.m_modelConstants)
	{
		m_settings.m_modelConstants = m_dirtySettings.m_modelConstants;

		ModelConstantsUpdated();
	}
}



//----------------------------------------------------------------------------------------------------------------------
void RendererInterface::UpdateCameraConstants(bool force)
{
	if (force || m_dirtySettings.m_cameraConstants != m_settings.m_cameraConstants)
	{
		m_settings.m_cameraConstants = m_dirtySettings.m_cameraConstants;

		CameraConstantsUpdated();
	}
}



//----------------------------------------------------------------------------------------------------------------------
void RendererInterface::UpdateFontConstants(bool force)
{
	if (force || m_dirtySettings.m_fontConstants != m_settings.m_fontConstants)
	{
		m_settings.m_fontConstants = m_dirtySettings.m_fontConstants;

		FontConstantsUpdated();
	}
}



//----------------------------------------------------------------------------------------------------------------------
void RendererInterface::UpdateBlendMode(bool force)
{
	if (force || m_dirtySettings.m_blendMode != m_settings.m_blendMode)
	{
		m_settings.m_blendMode = m_dirtySettings.m_blendMode;

		BlendModeUpdated();
	}
}



//----------------------------------------------------------------------------------------------------------------------
void RendererInterface::UpdateSamplerState(bool force)
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
void RendererInterface::UpdateTexture(bool force)
{
	if (force || m_dirtySettings.m_texture != m_settings.m_texture)
	{
		if (!m_dirtySettings.m_texture)
		{
			ASSERT_OR_DIE(m_defaultTexture, "No default texture available.");
			BindTexture(m_defaultTexture);
		}
		m_settings.m_texture = m_dirtySettings.m_texture;

		BoundTextureUpdated();
	}
}



//----------------------------------------------------------------------------------------------------------------------
void RendererInterface::UpdateShader(bool force)
{
	if (force || m_dirtySettings.m_shader != m_settings.m_shader)
	{
		if (m_dirtySettings.m_shader == nullptr)
		{
			ASSERT_OR_DIE(m_defaultShader, "No default shader exists.");
			BindShader(m_defaultShader);
		}
		m_settings.m_shader = m_dirtySettings.m_shader;

		BoundShaderUpdated();
	}
}



//----------------------------------------------------------------------------------------------------------------------
void RendererInterface::CreateConstantBuffers()
{
	m_cameraConstantsGPU = MakeConstantBuffer();
	m_modelConstantsGPU = MakeConstantBuffer();
	m_fontConstantsGPU = MakeConstantBuffer();

	m_cameraConstantsGPU->Initialize(sizeof(CameraConstants));
	m_modelConstantsGPU->Initialize(sizeof(ModelConstants));
	m_fontConstantsGPU->Initialize(sizeof(FontConstants));

	BindConstantBuffer(m_cameraConstantsGPU, 2);
	BindConstantBuffer(m_modelConstantsGPU, 3);
	BindConstantBuffer(m_fontConstantsGPU, 4);
}



//----------------------------------------------------------------------------------------------------------------------
void RendererInterface::DestroyConstantBuffers()
{
	if (m_cameraConstantsGPU)
	{
		m_cameraConstantsGPU->ReleaseResources();
		delete m_cameraConstantsGPU;
		m_cameraConstantsGPU = nullptr;
	}

	if (m_modelConstantsGPU)
	{
		m_modelConstantsGPU->ReleaseResources();
		delete m_modelConstantsGPU;
		m_modelConstantsGPU = nullptr;
	}

	if (m_fontConstantsGPU)
	{
		m_fontConstantsGPU->ReleaseResources();
		delete m_fontConstantsGPU;
		m_fontConstantsGPU = nullptr;
	}
}



//----------------------------------------------------------------------------------------------------------------------
void RendererInterface::DestroyDefaultShader()
{
	m_defaultShader->ReleaseResources();
	delete m_defaultShader;
	m_defaultShader = nullptr;
}



//----------------------------------------------------------------------------------------------------------------------
void RendererInterface::DestroyDefaultTexture()
{
	m_defaultTexture->ReleaseResources();
	delete m_defaultTexture;
	m_defaultTexture = nullptr;
}



//----------------------------------------------------------------------------------------------------------------------
void RendererInterface::DestroyDefaultFont()
{
	m_defaultFont->ReleaseResources();
	delete m_defaultFont;
	m_defaultFont = nullptr;
}



//----------------------------------------------------------------------------------------------------------------------
void RendererInterface::ModelConstantsUpdated()
{
	m_modelConstantsGPU->Update(&m_settings.m_modelConstants, sizeof(ModelConstants));
}



//----------------------------------------------------------------------------------------------------------------------
void RendererInterface::CameraConstantsUpdated()
{
	m_cameraConstantsGPU->Update(&m_settings.m_cameraConstants, sizeof(CameraConstants));
}



//----------------------------------------------------------------------------------------------------------------------
void RendererInterface::FontConstantsUpdated()
{
	m_fontConstantsGPU->Update(&m_settings.m_fontConstants, sizeof(FontConstants));
}



//----------------------------------------------------------------------------------------------------------------------
void RendererInterface::AddDevConsoleCommands()
{
	if (g_eventSystem)
	{
		g_eventSystem->SubscribeMethod("DebugDrawVertexBuffers", this, &RendererInterface::DebugDrawVertexBuffers);
		g_eventSystem->SubscribeMethod("ToggleVSync", this, &RendererInterface::ToggleVSync);
		g_eventSystem->SubscribeMethod("ToggleMSAA", this, &RendererInterface::ToggleMSAA);
	}
}



//----------------------------------------------------------------------------------------------------------------------
void RendererInterface::RemoveDevConsoleCommands()
{
	if (g_eventSystem)
	{
		g_eventSystem->UnsubscribeMethod("DebugDrawVertexBuffers", this, &RendererInterface::DebugDrawVertexBuffers);
		g_eventSystem->UnsubscribeMethod("ToggleVSync", this, &RendererInterface::ToggleVSync);
		g_eventSystem->UnsubscribeMethod("ToggleMSAA", this, &RendererInterface::ToggleMSAA);
	}
}



//----------------------------------------------------------------------------------------------------------------------
bool RendererInterface::DebugDrawVertexBuffers(NamedProperties&)
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
bool RendererInterface::ToggleVSync(NamedProperties&)
{
	m_perUserSettings.m_vsyncEnabled = !m_perUserSettings.m_vsyncEnabled;
	return false;
}



//----------------------------------------------------------------------------------------------------------------------
// TODO: Move factory function to its own file so the renderer interface doesnt have to have them all
//
#include "D3D11/D3D11Renderer.h"



//----------------------------------------------------------------------------------------------------------------------
RendererInterface* RendererInterface::MakeRendererInterface(RendererConfig const& config)
{
	return new D3D11Renderer(config);
}
