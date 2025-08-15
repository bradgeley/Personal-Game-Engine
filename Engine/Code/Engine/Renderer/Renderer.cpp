// Bradley Christensen - 2022-2023
#include "Renderer.h"
#include "Engine/Core/EngineCommon.h"
#include "DefaultShaderSource.h"
#include "RendererInternal.h"
#include "Shader.h"
#include "Texture.h"
#include "Window.h"
#include "Engine/Core/ErrorUtils.h"
#include "VertexBuffer.h"
#include "Camera.h"
#include "ConstantBuffer.h"
#include "Font.h"
#include "FontShaderSource.h"
#include "Engine/Debug/DebugDrawUtils.h"
#include "Engine/Debug/DevConsole.h"
#include "Engine/Performance/ScopedTimer.h"
#include "Engine/Events/EventSystem.h"
#include <thread>

#if defined(_DEBUG)
#include <dxgidebug.h>
#endif
#pragma comment(lib, "dxguid.lib")



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
	GetOrCreateWindowRenderContext(g_window);
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
	DestroyWindowRenderContexts();
	DestroyDevice();

	ReportLiveObjects();
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

	if (m_currentWindow)
	{
		EndWindow(m_currentWindow);
	}

	Present();
	std::this_thread::sleep_for(std::chrono::milliseconds(1));
}



//----------------------------------------------------------------------------------------------------------------------
void Renderer::BeginWindow(Window const* window)
{
	ASSERT_OR_DIE(window, "Renderer::BeginWindow - trying to begin a null window.");

	if (m_currentWindow == window)
	{
		return;
	}

	if (m_currentWindow && m_currentWindow != window)
	{
		EndWindow(m_currentWindow);
	}

	if (m_windowRenderContexts.find(window) != m_windowRenderContexts.end())
	{
		m_currentWindow = window;
		Texture*& texture = m_windowRenderContexts.at(m_currentWindow).m_backbufferTexture;
		BindRenderTarget(texture);
	}
	else
	{
		ERROR_AND_DIE("Tried to begin a window for which there is no window render context.")
	}
}



//----------------------------------------------------------------------------------------------------------------------
void Renderer::EndWindow(Window const* window)
{
	ASSERT_OR_DIE(window, "Renderer::EndWindow - trying to end a null window.");
	ASSERT_OR_DIE(window == m_currentWindow, "Renderer::EndWindow - window does not match m_currentWindow");

	m_currentWindow = nullptr;
}



//----------------------------------------------------------------------------------------------------------------------
Window const* Renderer::GetCurrentWindow() const
{
	return m_currentWindow;
}



//----------------------------------------------------------------------------------------------------------------------
void Renderer::BeginCamera(Camera const* camera)
{
	ASSERT_OR_DIE(camera, "Renderer::BeginCamera - trying to begin a null camera.");
	ASSERT_OR_DIE(m_deviceContext, "Renderer::BeginCamera - m_deviceContext is null");

	if (m_currentCamera && m_currentCamera != camera)
	{
		EndCamera(m_currentCamera);
	}

	if (!m_currentWindow)
	{
		BeginWindow(g_window);
	}

	m_currentCamera = camera;
	
	ResetRenderingPipelineState();

	// Fill Camera Constants
	m_dirtySettings.m_cameraConstants = camera->GetCameraConstants();
	
	// Set viewport
	IntVec2 renderResolution = m_currentWindow->GetRenderResolution();
	
	D3D11_VIEWPORT viewport;
	viewport.TopLeftX = 0.f;
	viewport.TopLeftY = 0.f;
	viewport.Width = static_cast<float>(renderResolution.x);
	viewport.Height = static_cast<float>(renderResolution.y);
	viewport.MinDepth = 0;
	viewport.MaxDepth = 1;
	m_deviceContext->RSSetViewports(1, &viewport);
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
void Renderer::Present() const
{
	// If multiple windows, just present them all
	for (auto& pair : m_windowRenderContexts)
	{
		WindowRenderContext const& wrc = pair.second;
		if (!wrc.m_window->m_config.m_automaticallyPresent)
		{
			continue;
		}
		if (wrc.m_window->GetIsMinimized())
		{
			// Suspend present while minimized
			continue;
		}

		PresentWindowRenderContext(wrc);
	}
}



//----------------------------------------------------------------------------------------------------------------------
void Renderer::PresentWindow(Window const* window) const
{
	WindowRenderContext const& wrc = GetWindowRenderContext(window);
	PresentWindowRenderContext(wrc);
}



//----------------------------------------------------------------------------------------------------------------------
void Renderer::PresentWindowRenderContext(WindowRenderContext const& wrc) const
{
	// Get the backbuffer texture
	ID3D11Texture2D* swapChainBackBufferTexture = nullptr;
	HRESULT hr = wrc.m_swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), reinterpret_cast<void**>(&swapChainBackBufferTexture));

	ASSERT_OR_DIE(SUCCEEDED(hr), "Swap chain did not have a backbuffer texture.")

	ID3D11DeviceContext* context = g_renderer->GetContext();

	// Paint onto the actual backbuffer texture using the texture that we've been rendering to at higher quality (lets us use MSAA)
	context->ResolveSubresource(
		swapChainBackBufferTexture, // actual backbuffer d3d texture
		0,
		wrc.m_backbufferTexture->m_textureHandle, // higher sample count texture that we've been rendering to
		0,
		DXGI_FORMAT_R8G8B8A8_UNORM
	);

	DX_SAFE_RELEASE(swapChainBackBufferTexture)

	UINT vsync = m_perUserSettings.m_vsyncEnabled ? 1 : 0;
	wrc.m_swapChain->Present(vsync, 0);
}



//----------------------------------------------------------------------------------------------------------------------
void Renderer::ClearScreen(Rgba8 const& tint)
{
	float colorAsFloats[4] = {};
	tint.GetAsFloats(&colorAsFloats[0]);
	ID3D11RenderTargetView* const renderTargetView = GetCurrentWindowRenderContext().m_backbufferTexture->CreateOrGetRenderTargetView();
	m_deviceContext->ClearRenderTargetView(renderTargetView, colorAsFloats);
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
void Renderer:: SetModelMatrix(Mat44 const& modelMatrix)
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
void Renderer::BindRenderTarget(Texture* texture)
{
	ID3D11DepthStencilView* dsv = GetCurrentWindowRenderContext().m_depthBuffer->CreateOrGetDepthStencilView();
	ID3D11RenderTargetView* rtv = texture->CreateOrGetRenderTargetView();
	m_deviceContext->OMSetRenderTargets(1, &rtv, dsv);
}



//----------------------------------------------------------------------------------------------------------------------
ID3D11Device* Renderer::GetDevice() const
{
	return m_device;
}



//----------------------------------------------------------------------------------------------------------------------
ID3D11DeviceContext* Renderer::GetContext() const
{
	return m_deviceContext;
}



//----------------------------------------------------------------------------------------------------------------------
void Renderer::ClearDepth(float depth)
{
	ID3D11DepthStencilView* view = GetCurrentWindowRenderContext().m_depthBuffer->CreateOrGetDepthStencilView();
	m_deviceContext->ClearDepthStencilView(view, D3D11_CLEAR_DEPTH, depth, 0);
}



//----------------------------------------------------------------------------------------------------------------------
void Renderer::BindVertexBuffer(VertexBuffer const* vbo) const
{
	UINT stride = (UINT) vbo->GetStride();
	UINT offset = 0;

	m_deviceContext->IASetVertexBuffers(
		0,
		1,
		&vbo->m_handle,
		&stride,
		&offset
	);

	ID3D11InputLayout* vertexLayout = m_defaultShader->CreateOrGetInputLayout();
	m_deviceContext->IASetInputLayout(vertexLayout);
}



//----------------------------------------------------------------------------------------------------------------------
void Renderer::BindConstantBuffer(ConstantBuffer const* cbo, int slot) const
{
	m_deviceContext->VSSetConstantBuffers(slot, 1, &cbo->m_handle);
	m_deviceContext->PSSetConstantBuffers(slot, 1, &cbo->m_handle);
}



//----------------------------------------------------------------------------------------------------------------------
RendererPerUserSettings Renderer::GetPerUserSettings() const
{
	return m_perUserSettings;
}



//----------------------------------------------------------------------------------------------------------------------
Font* Renderer::GetDefaultFont() const
{
	return m_defaultFont;
}



//----------------------------------------------------------------------------------------------------------------------
void Renderer::Draw(int vertexCount, int vertexOffset)
{
	UpdateRenderingPipelineState();
	m_deviceContext->Draw(vertexCount, vertexOffset);
	++m_numFrameDrawCalls;
}



//----------------------------------------------------------------------------------------------------------------------
void Renderer::CreateDevice()
{
	UINT deviceFlags = 0;
	#ifdef _DEBUG
		deviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
	#endif

	HRESULT hr = D3D11CreateDevice(
		nullptr,                    // default adapter
		D3D_DRIVER_TYPE_HARDWARE,   // hardware driver
		nullptr,                    // no software rasterizer
		deviceFlags,                // flags (e.g.,  D3D11_CREATE_DEVICE_DEBUG)
		nullptr,					// feature levels array
		0,							// number of feature levels
		D3D11_SDK_VERSION,          // SDK version
		&m_device,                  // output device pointer
		nullptr,					// output feature level
		&m_deviceContext            // output immediate context
	);

	ASSERT_OR_DIE(SUCCEEDED(hr), "D3D11CreateDevice failed with result");
	ASSERT_OR_DIE(m_device, "Failed to create device");
	ASSERT_OR_DIE(m_deviceContext, "Failed to create device context");

	#if defined(_DEBUG)
		std::string deviceName = StringUtils::StringF("Device (THE Renderer)");
		m_device->SetPrivateData(WKPDID_D3DDebugObjectName, (int) deviceName.size(), deviceName.data());
		std::string contextName = StringUtils::StringF("Device Context (THE Renderer)");
		m_deviceContext->SetPrivateData(WKPDID_D3DDebugObjectName, (int) contextName.size(), contextName.data());
	#endif
}



//----------------------------------------------------------------------------------------------------------------------
void Renderer::CreateDebugLayer()
{
#if defined(_DEBUG)
	HMODULE modHandle = ::LoadLibraryA("Dxgidebug.dll");
	ASSERT_OR_DIE(modHandle, "Failed to load Dxgidebug.dll")

	typedef HRESULT(WINAPI* GetDebugModuleFunc)(REFIID, void**);
	GetDebugModuleFunc getModuleFunc = (GetDebugModuleFunc) ::GetProcAddress(modHandle, "DXGIGetDebugInterface");
	ASSERT_OR_DIE(getModuleFunc, "Failed to acquire DXGIGetDebugInterface")

	HRESULT result = getModuleFunc(__uuidof(IDXGIDebug), (void**) &m_debug);
	ASSERT_OR_DIE(SUCCEEDED(result), "getModuleFunc Failed")

	m_debugModule = modHandle;
#endif
}



//----------------------------------------------------------------------------------------------------------------------
void Renderer::DestroyDebugLayer()
{
#if defined(_DEBUG)
	if (m_debugModule)
	{
		DX_SAFE_RELEASE(m_debug)
		::FreeLibrary((HMODULE) m_debugModule);
		m_debugModule = nullptr;
	}
#endif
}



//----------------------------------------------------------------------------------------------------------------------
void Renderer::ReportLiveObjects()
{
#if defined(_DEBUG)
	if (m_debug)
	{
		DXGI_DEBUG_RLO_FLAGS flags = (DXGI_DEBUG_RLO_FLAGS) (DXGI_DEBUG_RLO_DETAIL | DXGI_DEBUG_RLO_IGNORE_INTERNAL);
		m_debug->ReportLiveObjects(DXGI_DEBUG_ALL, flags);
	}
#endif
}



//----------------------------------------------------------------------------------------------------------------------
ID3D11SamplerState* Renderer::CreateSamplerState(SamplerFilter filter, SamplerAddressMode addressMode)
{
	float maxAnisotropy = 0.f;
	D3D11_FILTER d3d11Filter = GetD3D11SamplerFilter(filter, maxAnisotropy);
	D3D11_TEXTURE_ADDRESS_MODE d3d11AddressMode = GetD3D11SamplerAddressMode(addressMode);

	D3D11_SAMPLER_DESC desc = {};
	desc.Filter = d3d11Filter;
 	desc.AddressU = d3d11AddressMode;
	desc.AddressV = d3d11AddressMode;
	desc.AddressW = d3d11AddressMode;

	desc.MipLODBias = 0.f;
	desc.MinLOD = 0.f;
	desc.MaxLOD = D3D11_FLOAT32_MAX;
	desc.MaxAnisotropy = (UINT) maxAnisotropy;
	desc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
	
	ID3D11SamplerState* samplerState = nullptr;
	HRESULT result = m_device->CreateSamplerState(&desc, &samplerState);
	ASSERT_OR_DIE(SUCCEEDED(result), "Failed to create sampler state.")
	return samplerState;
}



//----------------------------------------------------------------------------------------------------------------------
void Renderer::DestroySamplerStates()
{
	DX_SAFE_RELEASE(m_samplerState)
}



//----------------------------------------------------------------------------------------------------------------------
WindowRenderContext& Renderer::GetOrCreateWindowRenderContext(Window* window)
{
	auto existingWRC = m_windowRenderContexts.find(window);
	if (existingWRC != m_windowRenderContexts.end())
	{
		return existingWRC->second;
	}
	
	IDXGIDevice* device;
	m_device->QueryInterface(__uuidof(IDXGIDevice), (void**)&device);
	
	IDXGIAdapter* adapter;
	device->GetParent(__uuidof(IDXGIAdapter), (void**)&adapter);
	
	IDXGIFactory* factory;
	adapter->GetParent(__uuidof(IDXGIFactory), (void**)&factory);

	IntVec2 renderResolution = window->GetRenderResolution();
	
	DXGI_SWAP_CHAIN_DESC swapChainDesc = {};
	swapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	swapChainDesc.BufferDesc.Height = renderResolution.y;
	swapChainDesc.BufferDesc.Width = renderResolution.x;
	swapChainDesc.BufferDesc.RefreshRate = { 0, 0 };
	swapChainDesc.Windowed = TRUE;
	swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
	swapChainDesc.Flags = 0;
	swapChainDesc.BufferCount = 2;
	swapChainDesc.OutputWindow = (HWND) window->GetHWND();
	swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	swapChainDesc.SampleDesc.Count = 1;

	WindowRenderContext& windowRenderContext = m_windowRenderContexts[window];
	windowRenderContext.m_window = window;
	
	HRESULT result = factory->CreateSwapChain(m_device, &swapChainDesc, &windowRenderContext.m_swapChain);
	if (!SUCCEEDED(result))
	{
		g_devConsole->LogError("Renderer::CreateRenderContextForWindow: Failed to create swap chain");
		return windowRenderContext;
	}

	DX_SAFE_RELEASE(factory)
	DX_SAFE_RELEASE(adapter)
	DX_SAFE_RELEASE(device)
	
	windowRenderContext.m_backbufferTexture = new Texture();
	windowRenderContext.m_backbufferTexture->InitAsBackbufferTexture(windowRenderContext.m_swapChain);

	windowRenderContext.m_depthBuffer = new Texture();
	windowRenderContext.m_depthBuffer->InitAsDepthBuffer(windowRenderContext.m_swapChain);

	window->m_focusChanged.SubscribeMethod(this, &Renderer::WindowFocusChanged);
	window->m_windowModeChanged.SubscribeMethod(this, &Renderer::WindowModeChanged);
	window->m_windowSizeChanged.SubscribeMethod(this, &Renderer::WindowSizeChanged);

	return windowRenderContext;
}



//----------------------------------------------------------------------------------------------------------------------
WindowRenderContext& Renderer::GetCurrentWindowRenderContext()
{
	return m_windowRenderContexts.at(m_currentWindow);
}



//----------------------------------------------------------------------------------------------------------------------
WindowRenderContext const& Renderer::GetCurrentWindowRenderContext() const
{
	return m_windowRenderContexts.at(m_currentWindow);
}



//----------------------------------------------------------------------------------------------------------------------
WindowRenderContext& Renderer::GetWindowRenderContext(Window const* window)
{
	return m_windowRenderContexts.at(window);
}



//----------------------------------------------------------------------------------------------------------------------
WindowRenderContext const& Renderer::GetWindowRenderContext(Window const* window) const
{
	return m_windowRenderContexts.at(window);
}



//----------------------------------------------------------------------------------------------------------------------
MSAASettings Renderer::GetMaxSupportedMSAASettings(DXGI_FORMAT format)
{
	MSAASettings result;
	if (!m_perUserSettings.m_msaaEnabled)
	{
		return result;
	}

	for (int sampleCount = D3D10_MAX_MULTISAMPLE_SAMPLE_COUNT; sampleCount >= 1; sampleCount /= 2)
	{
		UINT numQualityLevels = 0;
		HRESULT hr = m_device->CheckMultisampleQualityLevels(
			format,
			sampleCount,
			&numQualityLevels
		);

		if (SUCCEEDED(hr) && numQualityLevels >= 1)
		{
			result.m_sampleCount = sampleCount;
			result.m_qualityLevel = numQualityLevels - 1;
			break;
		}
	}

	return result;
}



//----------------------------------------------------------------------------------------------------------------------
int Renderer::GetNumFrameDrawCalls() const
{
	return m_numFrameDrawCalls;
}



//----------------------------------------------------------------------------------------------------------------------
void Renderer::CreateDefaultShader()
{
	ShaderConfig defaultShaderConfig;
	defaultShaderConfig.m_name = "DefaultShader";
	m_defaultShader = new Shader(defaultShaderConfig);
	m_defaultShader->CreateFromSource(s_defaultShaderSource);
}



//----------------------------------------------------------------------------------------------------------------------
void Renderer::DestroyDefaultShader()
{
	m_defaultShader->ReleaseResources();
	delete m_defaultShader;
	m_defaultShader = nullptr;
}



//----------------------------------------------------------------------------------------------------------------------
void Renderer::CreateDefaultTexture()
{
	m_defaultTexture = new Texture();
	m_defaultTexture->CreateUniformTexture(IntVec2(1, 1), Rgba8::White);
}



//----------------------------------------------------------------------------------------------------------------------
void Renderer::DestroyDefaultTexture()
{
	m_defaultTexture->ReleaseResources();
	delete m_defaultTexture;
	m_defaultTexture = nullptr;
}



//----------------------------------------------------------------------------------------------------------------------
void Renderer::CreateDefaultFont()
{
	m_defaultFont = new Font();
	m_defaultFont->LoadFNT("Data/Fonts/Gypsy.fnt");
	ShaderConfig config;
	config.m_name = "Default Font Shader";
	m_defaultFont->m_shader = new Shader(config);
	m_defaultFont->m_shader->CreateFromSource(s_defaultFontShaderSource);
}



//----------------------------------------------------------------------------------------------------------------------
void Renderer::DestroyDefaultFont()
{
	delete m_defaultFont;
	m_defaultFont = nullptr;
}



//----------------------------------------------------------------------------------------------------------------------
void Renderer::CreateRasterizerState()
{
	DX_SAFE_RELEASE(m_rasterizerState)
	
	D3D11_RASTERIZER_DESC rasterizerDesc;
	rasterizerDesc.FillMode = GetD3D11FillMode(m_settings.m_fillMode);
	rasterizerDesc.CullMode = GetD3D11CullMode(m_settings.m_cullMode);
	rasterizerDesc.FrontCounterClockwise = m_settings.m_winding == Winding::CounterClockwise ? true : false;

	rasterizerDesc.DepthBias = 0;
	rasterizerDesc.DepthBiasClamp = 0.f;
	rasterizerDesc.SlopeScaledDepthBias = 0.f;

	rasterizerDesc.ScissorEnable = false;
	rasterizerDesc.MultisampleEnable = false;
	rasterizerDesc.AntialiasedLineEnable = true;
	rasterizerDesc.DepthClipEnable = true;

	m_device->CreateRasterizerState(&rasterizerDesc, &m_rasterizerState);
	ASSERT_OR_DIE(m_rasterizerState, "Failed to create rasterizer state")
}



//----------------------------------------------------------------------------------------------------------------------
void Renderer::DestroyRasterizerState()
{
	DX_SAFE_RELEASE(m_rasterizerState)
}



//----------------------------------------------------------------------------------------------------------------------
void Renderer::DestroyDepthStencilState()
{
	DX_SAFE_RELEASE(m_depthStencilState)
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
void Renderer::CreateConstantBuffers()
{
	m_cameraConstantsGPU = new ConstantBuffer();
	m_modelConstantsGPU = new ConstantBuffer();
	m_fontConstantsGPU = new ConstantBuffer();

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
	m_cameraConstantsGPU->ReleaseResources();
	delete m_cameraConstantsGPU;
	m_cameraConstantsGPU = nullptr;

	m_modelConstantsGPU->ReleaseResources();
	delete m_modelConstantsGPU;
	m_modelConstantsGPU = nullptr;
	
	m_fontConstantsGPU->ReleaseResources();
	delete m_fontConstantsGPU;
	m_fontConstantsGPU = nullptr;
}



//----------------------------------------------------------------------------------------------------------------------
void Renderer::CreateBlendStates()
{
	m_blendStateByMode[(int) BlendMode::Opaque] = CreateBlendState(D3D11_BLEND_ONE, D3D11_BLEND_ZERO, D3D11_BLEND_OP_ADD);
	m_blendStateByMode[(int) BlendMode::Alpha] = CreateBlendState(D3D11_BLEND_SRC_ALPHA, D3D11_BLEND_INV_SRC_ALPHA, D3D11_BLEND_OP_ADD);
	m_blendStateByMode[(int) BlendMode::Additive] = CreateBlendState(D3D11_BLEND_ONE, D3D11_BLEND_ONE, D3D11_BLEND_OP_ADD);

	#ifdef _DEBUG
		std::string opaqueName = StringUtils::StringF("Blend State (Opaque)");
		m_blendStateByMode[(int) BlendMode::Opaque]->SetPrivateData(WKPDID_D3DDebugObjectName, (int) opaqueName.size(), opaqueName.data());
		std::string alphaName = StringUtils::StringF("Blend State (Alpha)");
		m_blendStateByMode[(int) BlendMode::Alpha]->SetPrivateData(WKPDID_D3DDebugObjectName, (int) alphaName.size(), alphaName.data());
		std::string additiveName = StringUtils::StringF("Blend State (Additive)");
		m_blendStateByMode[(int) BlendMode::Additive]->SetPrivateData(WKPDID_D3DDebugObjectName, (int) additiveName.size(), additiveName.data());
	#endif
	
	// Default Blend Mode, move to separate default settings function?
	SetBlendMode(BlendMode::Alpha);
}



//----------------------------------------------------------------------------------------------------------------------
void Renderer::DestroyDevice()
{
	m_deviceContext->ClearState();
	m_deviceContext->Flush();
	
	DX_SAFE_RELEASE(m_deviceContext)
	DX_SAFE_RELEASE(m_device)
}



//----------------------------------------------------------------------------------------------------------------------
void Renderer::DestroyWindowRenderContexts()
{
	for (auto pair : m_windowRenderContexts)
	{
		WindowRenderContext& wrc = pair.second;

		wrc.m_window->m_focusChanged.UnsubscribeMethod(this, &Renderer::WindowFocusChanged);
		wrc.m_window->m_windowModeChanged.UnsubscribeMethod(this, &Renderer::WindowModeChanged);
		wrc.m_window->m_windowSizeChanged.UnsubscribeMethod(this, &Renderer::WindowSizeChanged);

		wrc.m_swapChain->SetFullscreenState(false, nullptr);

		if (wrc.m_backbufferTexture)
		{
			wrc.m_backbufferTexture->ReleaseResources();
			delete wrc.m_backbufferTexture;
			wrc.m_backbufferTexture = nullptr;
		}

		if (wrc.m_depthBuffer)
		{
			wrc.m_depthBuffer->ReleaseResources();
			delete wrc.m_depthBuffer;
			wrc.m_depthBuffer = nullptr;
		}

		DX_SAFE_RELEASE(wrc.m_swapChain)
	}

	m_windowRenderContexts.clear();
}



//----------------------------------------------------------------------------------------------------------------------
ID3D11BlendState* Renderer::CreateBlendState(::D3D11_BLEND srcFactor, ::D3D11_BLEND dstFactor, ::D3D11_BLEND_OP op)
{
	D3D11_BLEND_DESC blendDesc;
	blendDesc.AlphaToCoverageEnable = false;
	blendDesc.IndependentBlendEnable = false;
	blendDesc.RenderTarget[0].BlendEnable = true;
	blendDesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

	blendDesc.RenderTarget[0].SrcBlend = srcFactor;
	blendDesc.RenderTarget[0].DestBlend = dstFactor;
	blendDesc.RenderTarget[0].BlendOp = op;

	blendDesc.RenderTarget[0].SrcBlendAlpha = srcFactor;
	blendDesc.RenderTarget[0].DestBlendAlpha = dstFactor;
	blendDesc.RenderTarget[0].BlendOpAlpha = op;

	ID3D11BlendState* blendState = nullptr;
	HRESULT result = m_device->CreateBlendState(
		&blendDesc,
		&blendState
	);

	ASSERT_OR_DIE(SUCCEEDED(result), "Failed to create blend state")
	
	return blendState;
}



//----------------------------------------------------------------------------------------------------------------------
bool Renderer::DebugDrawVertexBuffers(NamedProperties& args)
{
UNUSED(args)

#if defined(_DEBUG)
m_debugDrawVertexBuffers = !m_debugDrawVertexBuffers;
return true;
#else
g_devConsole->LogError("Cannot debug draw vertex buffers in a Release build.");
return false;
#endif
}



//----------------------------------------------------------------------------------------------------------------------
bool Renderer::ToggleVSync(NamedProperties& args)
{
	UNUSED(args)
		m_perUserSettings.m_vsyncEnabled = !m_perUserSettings.m_vsyncEnabled;
	return false;
}



//----------------------------------------------------------------------------------------------------------------------
bool Renderer::ToggleMSAA(NamedProperties& args)
{
	UNUSED(args)
		m_perUserSettings.m_msaaEnabled = !m_perUserSettings.m_msaaEnabled;

	// Recreate the depth buffer and backbuffer textures
	for (auto& wrcPair : m_windowRenderContexts)
	{
		WindowRenderContext& wrc = wrcPair.second;
		wrc.m_backbufferTexture->ReleaseResources();
		wrc.m_depthBuffer->ReleaseResources();

		wrc.m_backbufferTexture = new Texture();
		wrc.m_backbufferTexture->InitAsBackbufferTexture(wrc.m_swapChain);

		wrc.m_depthBuffer = new Texture();
		wrc.m_depthBuffer->InitAsDepthBuffer(wrc.m_swapChain);
	}

	return false;
}



//----------------------------------------------------------------------------------------------------------------------
bool Renderer::WindowFocusChanged(NamedProperties& args)
{
	Window* window = args.Get("window", (Window*) nullptr);
	if (!window)
	{
		return false;
	}

	WindowRenderContext& wrc = GetOrCreateWindowRenderContext(window);

	if (window->IsFullscreen())
	{
		if (!window->HasFocus())
		{
			window->Minimize();
		}
		else
		{
			ShowWindow((HWND) window->GetHWND(), SW_SHOW);
		}
	}
	else
	{
		if (window->HasFocus())
		{
			SetForegroundWindow((HWND) window->GetHWND());
			SetActiveWindow((HWND) window->GetHWND());
		}
	}

	return false;
}



//----------------------------------------------------------------------------------------------------------------------
bool Renderer::WindowModeChanged(NamedProperties& args)
{
	Window* window = args.Get("window", (Window*) nullptr);
	if (!window)
	{
		return false;
	}

	WindowMode previousMode = args.Get("previousMode", WindowMode::Borderless);
	WindowMode mode = args.Get("mode", WindowMode::Borderless);
	WindowRenderContext& wrc = GetOrCreateWindowRenderContext(window);

	HWND hwnd = (HWND) window->GetHWND();

	// Restore window if minimized
	WINDOWPLACEMENT wp = {};
	wp.length = sizeof(wp);
	GetWindowPlacement(hwnd, &wp);
	if (wp.showCmd == SW_SHOWMINIMIZED)
	{
		wp.showCmd = SW_SHOWNORMAL;
		SetWindowPlacement(hwnd, &wp);
	}

	// Get monitor rect
	RECT monitorRect = {};
	HMONITOR hMon = MonitorFromWindow(hwnd, MONITOR_DEFAULTTOPRIMARY);
	MONITORINFO mi = { sizeof(mi) };
	GetMonitorInfo(hMon, &mi);
	monitorRect = mi.rcMonitor;

	HRESULT fullscreenResult;
	if (!window->IsFullscreen())
	{
		fullscreenResult = wrc.m_swapChain->SetFullscreenState(FALSE, nullptr);
	}

	// Set style
	DWORD style = window->GetWindowStyleFlags();
	SetWindowLongPtr(hwnd, GWL_STYLE, style);

	RECT targetRect = monitorRect;
	if (window->IsWindowed())
	{
		IntVec2 lastKnownPos = window->GetLastKnownWindowedPos();
		IntVec2 desiredResolution = window->GetDesiredWindowResolution(); 			
		targetRect.left = lastKnownPos.x;
		targetRect.top = lastKnownPos.y;
		targetRect.right = targetRect.left + desiredResolution.x;
		targetRect.bottom = targetRect.top + desiredResolution.y;
		bool adjusted = AdjustWindowRect(&targetRect, style, FALSE);

		ASSERT_OR_DIE(adjusted, "Failed to adjust");
	}

	std::this_thread::yield();
	bool setPos = SetWindowPos(hwnd, HWND_TOP,
				 targetRect.left, targetRect.top,
				 targetRect.right - targetRect.left,
				 targetRect.bottom - targetRect.top,
				 SWP_FRAMECHANGED | SWP_SHOWWINDOW);

	ASSERT_OR_DIE(setPos, "Failed to set window Pos");

	if (window->IsFullscreen())
	{
		fullscreenResult = wrc.m_swapChain->SetFullscreenState(TRUE, nullptr);
	}

	ASSERT_OR_DIE(SUCCEEDED(fullscreenResult), "Failed to set fullscreen.");

	ResizeWindowRenderContext(wrc);

	return false;
}



//----------------------------------------------------------------------------------------------------------------------
bool Renderer::WindowSizeChanged(NamedProperties& args)
{
	Window* window = args.Get("window", (Window*) nullptr);
	if (!window)
	{
		return false;
	}

	WindowRenderContext& wrc = GetOrCreateWindowRenderContext(window);

	ResizeWindowRenderContext(wrc);

	return false;
}



//----------------------------------------------------------------------------------------------------------------------
void Renderer::DestroyBlendStates()
{
	DX_SAFE_RELEASE(m_blendStateByMode[(int) BlendMode::Opaque])
	DX_SAFE_RELEASE(m_blendStateByMode[(int) BlendMode::Additive])
	DX_SAFE_RELEASE(m_blendStateByMode[(int) BlendMode::Alpha])
}



//----------------------------------------------------------------------------------------------------------------------
void Renderer::UpdateRasterizerState(bool force)
{
	if (force ||
		m_dirtySettings.m_cullMode != m_settings.m_cullMode ||
		m_dirtySettings.m_winding  != m_settings.m_winding  ||
		m_dirtySettings.m_fillMode != m_settings.m_fillMode)
	{
		m_settings.m_cullMode = m_dirtySettings.m_cullMode;
		m_settings.m_winding = m_dirtySettings.m_winding;
		m_settings.m_fillMode = m_dirtySettings.m_fillMode;
		
		DX_SAFE_RELEASE(m_rasterizerState)
		CreateRasterizerState();
		m_deviceContext->RSSetState(m_rasterizerState);
	}
}



//----------------------------------------------------------------------------------------------------------------------
void Renderer::UpdateDepthStencilState(bool force)
{
	if (force ||
		m_dirtySettings.m_writeDepth != m_settings.m_writeDepth ||
		m_dirtySettings.m_depthTest  != m_settings.m_depthTest)
	{
		m_settings.m_writeDepth = m_dirtySettings.m_writeDepth;
		m_settings.m_depthTest = m_dirtySettings.m_depthTest;
		
		DX_SAFE_RELEASE(m_depthStencilState)
	
		D3D11_DEPTH_STENCIL_DESC desc;
		desc.DepthEnable = TRUE;
		desc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
		desc.DepthFunc = D3D11_COMPARISON_ALWAYS;
		desc.StencilEnable = FALSE;
	
		m_device->CreateDepthStencilState(&desc, &m_depthStencilState);
		m_deviceContext->OMSetDepthStencilState(m_depthStencilState, 0);
	}
}



//----------------------------------------------------------------------------------------------------------------------
void Renderer::UpdateModelConstants(bool force)
{
	if (force || m_dirtySettings.m_modelConstants != m_settings.m_modelConstants)
	{
		m_settings.m_modelConstants = m_dirtySettings.m_modelConstants;
		m_modelConstantsGPU->Update(&m_settings.m_modelConstants, sizeof(ModelConstants));
	}
}



//----------------------------------------------------------------------------------------------------------------------
void Renderer::UpdateCameraConstants(bool force)
{
	if (force || m_dirtySettings.m_cameraConstants != m_settings.m_cameraConstants)
	{
		m_settings.m_cameraConstants = m_dirtySettings.m_cameraConstants;
		m_cameraConstantsGPU->Update(&m_settings.m_cameraConstants, sizeof(CameraConstants));
	}
}



//----------------------------------------------------------------------------------------------------------------------
void Renderer::UpdateFontConstants(bool force)
{
	if (force || m_dirtySettings.m_fontConstants != m_settings.m_fontConstants)
	{
		m_settings.m_fontConstants = m_dirtySettings.m_fontConstants;
		m_fontConstantsGPU->Update(&m_settings.m_fontConstants, sizeof(FontConstants));
	}
}



//----------------------------------------------------------------------------------------------------------------------
void Renderer::UpdateBlendMode(bool force)
{
	if (force || m_dirtySettings.m_blendMode != m_settings.m_blendMode)
	{
		m_settings.m_blendMode = m_dirtySettings.m_blendMode;
		ASSERT_OR_DIE(m_blendStateByMode[(int) m_settings.m_blendMode], "SetBlendMode Failed due to invalid blend state.")
		float constexpr blendConstants[4] = { 0.f, 0.f, 0.f, 0.f };
		m_deviceContext->OMSetBlendState(m_blendStateByMode[(int) m_settings.m_blendMode], blendConstants, 0xffffffff);
	}
}



//----------------------------------------------------------------------------------------------------------------------
void Renderer::UpdateSamplerState(bool force)
{
	if (force ||
		m_dirtySettings.m_samplerFilter	!= m_settings.m_samplerFilter ||
		m_dirtySettings.m_samplerAddressMode != m_settings.m_samplerAddressMode)
	{
		m_settings.m_samplerFilter = m_dirtySettings.m_samplerFilter;
		m_settings.m_samplerAddressMode = m_dirtySettings.m_samplerAddressMode;
		
		DX_SAFE_RELEASE(m_samplerState)
		
		m_samplerState = CreateSamplerState(m_settings.m_samplerFilter, m_settings.m_samplerAddressMode);
		ASSERT_OR_DIE(m_samplerState, "Failed to create or get sampler state")

		// todo: support multiple samplers at a time
		m_deviceContext->PSSetSamplers(0, 1, &m_samplerState);
	}
}



//----------------------------------------------------------------------------------------------------------------------
void Renderer::UpdateTexture(bool force)
{
	if (force || m_dirtySettings.m_texture != m_settings.m_texture)
	{
		if (!m_dirtySettings.m_texture)
		{
			ASSERT_OR_DIE(m_defaultTexture, "No default texture available.")
			BindTexture(m_defaultTexture);
		}
		m_settings.m_texture = m_dirtySettings.m_texture;

		ID3D11ShaderResourceView* srv = m_settings.m_texture->CreateOrGetShaderResourceView();

		// todo: support multiple textures bound at the same time
		m_deviceContext->PSSetShaderResources(0, 1, &srv);
	}
}



//----------------------------------------------------------------------------------------------------------------------
void Renderer::UpdateShader(bool force)
{
	if (force || m_dirtySettings.m_shader != m_settings.m_shader)
	{
		if (m_dirtySettings.m_shader == nullptr)
		{
			ASSERT_OR_DIE(m_defaultShader, "No default shader exists.")
			BindShader(m_defaultShader);
		}
		m_settings.m_shader = m_dirtySettings.m_shader;

		m_deviceContext->VSSetShader(m_settings.m_shader->m_vertexShader, nullptr, 0);
		m_deviceContext->PSSetShader(m_settings.m_shader->m_pixelShader, nullptr, 0);
		m_deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	}
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
void Renderer::ResizeWindowRenderContext(WindowRenderContext& wrc)
{
	wrc.m_backbufferTexture->ReleaseResources();
	wrc.m_depthBuffer->ReleaseResources();

	DXGI_SWAP_CHAIN_DESC desc = {};
	wrc.m_swapChain->GetDesc(&desc);

	IntVec2 renderResolution = wrc.m_window->GetRenderResolution();

	HRESULT hr = wrc.m_swapChain->ResizeBuffers(
		desc.BufferCount,
		renderResolution.x,
		renderResolution.y,
		desc.BufferDesc.Format,
		desc.Flags
	);

	ASSERT_OR_DIE(SUCCEEDED(hr), "Failed to resize buffers after window mode change.");

	wrc.m_backbufferTexture->InitAsBackbufferTexture(wrc.m_swapChain);
	wrc.m_depthBuffer->InitAsDepthBuffer(wrc.m_swapChain);
}



