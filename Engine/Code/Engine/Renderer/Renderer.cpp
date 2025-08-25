// Bradley Christensen - 2022-2023
#include "Renderer.h"
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
Renderer* g_renderer = nullptr;



//----------------------------------------------------------------------------------------------------------------------
uint32_t INVALID_RENDER_TARGET_ID = UINT32_MAX;



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
}



//----------------------------------------------------------------------------------------------------------------------
void Renderer::Render() const
{

}



//----------------------------------------------------------------------------------------------------------------------
void Renderer::Shutdown()
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

	if (m_currentRenderTarget != INVALID_RENDER_TARGET_ID)
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
RendererPerUserSettings Renderer::GetPerUserSettings() const
{
    return m_perUserSettings;
}



//----------------------------------------------------------------------------------------------------------------------
void Renderer::DrawVertexBuffer(VertexBuffer* vbo)
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
void Renderer::BindTexture(Texture* texture)
{
	m_dirtySettings.m_texture = texture;
}



//----------------------------------------------------------------------------------------------------------------------
void Renderer::BindShader(Shader* shader)
{
	m_dirtySettings.m_shader = shader;
}



//----------------------------------------------------------------------------------------------------------------------
void Renderer::UnbindRenderTarget(RenderTargetID)
{
	m_currentRenderTarget = INVALID_RENDER_TARGET_ID;
}



//----------------------------------------------------------------------------------------------------------------------
int Renderer::GetNumFrameDrawCalls() const
{
	return m_numFrameDrawCalls;
}



//----------------------------------------------------------------------------------------------------------------------
Font* Renderer::GetDefaultFont() const
{
	return m_defaultFont;
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
void Renderer::UpdateShader(bool force)
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
void Renderer::CreateConstantBuffers()
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
void Renderer::DestroyConstantBuffers()
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
void Renderer::DestroyDefaultShader()
{
	m_defaultShader->ReleaseResources();
	delete m_defaultShader;
	m_defaultShader = nullptr;
}



//----------------------------------------------------------------------------------------------------------------------
void Renderer::DestroyDefaultTexture()
{
	m_defaultTexture->ReleaseResources();
	delete m_defaultTexture;
	m_defaultTexture = nullptr;
}



//----------------------------------------------------------------------------------------------------------------------
void Renderer::DestroyDefaultFont()
{
	m_defaultFont->ReleaseResources();
	delete m_defaultFont;
	m_defaultFont = nullptr;
}



//----------------------------------------------------------------------------------------------------------------------
void Renderer::ModelConstantsUpdated()
{
	m_modelConstantsGPU->Update(&m_settings.m_modelConstants, sizeof(ModelConstants));
}



//----------------------------------------------------------------------------------------------------------------------
void Renderer::CameraConstantsUpdated()
{
	m_cameraConstantsGPU->Update(&m_settings.m_cameraConstants, sizeof(CameraConstants));
}



//----------------------------------------------------------------------------------------------------------------------
void Renderer::FontConstantsUpdated()
{
	m_fontConstantsGPU->Update(&m_settings.m_fontConstants, sizeof(FontConstants));
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
Renderer* Renderer::MakeRendererInterface(RendererConfig const& config)
{
	return new D3D11Renderer(config);
}
