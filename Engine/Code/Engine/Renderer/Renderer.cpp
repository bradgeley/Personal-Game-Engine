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
#include <thread>
#include "Camera.h"
#include "ConstantBuffer.h"
#include "Font.h"
#include "FontShaderSource.h"

#if defined(_DEBUG)
#include <dxgidebug.h>
#endif
#pragma comment( lib, "dxguid.lib")



//----------------------------------------------------------------------------------------------------------------------
// THE Renderer
//
Renderer* g_renderer = nullptr;



//----------------------------------------------------------------------------------------------------------------------
Renderer::Renderer(RendererConfig const& config) : m_config(config)
{
    
}



//----------------------------------------------------------------------------------------------------------------------
void Renderer::Startup()
{
	CreateDebugLayer();
	CreateRenderContext();
	CreateDefaultShader();
	CreateDefaultTexture();
	CreateBlendStates();
	CreateRasterizerState();
	CreateDepthBuffer();
	CreateConstantBuffers();
	CreateDefaultFont();

	UpdateRenderingPipelineState(true);
}



//----------------------------------------------------------------------------------------------------------------------
void Renderer::Shutdown()
{
	DestroyDefaultFont();
	DestroyDefaultShader();
	DestroyDefaultTexture();
	DestroyBlendStates();
	DestroyRasterizerState();
	DestroyDepthBuffer();
	DestroyConstantBuffers();
	DestroySamplerStates();
	DestroyDepthStencilState();
	DestroyRenderContext();

	ReportLiveObjects();
	DestroyDebugLayer();
}


//----------------------------------------------------------------------------------------------------------------------
void Renderer::BeginFrame()
{
	ClearDepth(1.f);
}



//----------------------------------------------------------------------------------------------------------------------
void Renderer::EndFrame()
{
	m_swapChain->Present(0, 0);

	// todo: handle fast framerate issues
	std::this_thread::sleep_for(std::chrono::milliseconds(16));
}



//----------------------------------------------------------------------------------------------------------------------
void Renderer::BeginCamera(Camera const& camera)
{
	ResetRenderingPipelineState();
	
	BindRenderTarget(m_backbufferTexture);

	// Fill Camera Constants
	m_dirtySettings.m_cameraConstants.m_cameraToClip = camera.GetOrthoProjectionMatrix();
	
	// Set viewport
	IntVec2 windowDims = g_window->GetDimensions();
	Vec2 botLeft = Vec2::ZeroVector;
	Vec2 topRight = Vec2(windowDims.x, windowDims.y);
	
	D3D11_VIEWPORT viewport;
	viewport.TopLeftX = botLeft.x;
	viewport.TopLeftY = botLeft.y;
	viewport.Width = topRight.x;
	viewport.Height = topRight.y;
	viewport.MinDepth = 0;
	viewport.MaxDepth = 1;
	m_deviceContext->RSSetViewports(1, &viewport);
}



//----------------------------------------------------------------------------------------------------------------------
void Renderer::EndCamera(Camera const& camera)
{
	UNUSED(camera)
}



//----------------------------------------------------------------------------------------------------------------------
void Renderer::ClearScreen(Rgba8 const& tint)
{
	float colorAsFloats[4] = {};
	tint.GetAsFloats(&colorAsFloats[0]);
	BindRenderTarget(m_backbufferTexture);
	ID3D11RenderTargetView* const renderTargetView = m_backbufferTexture->CreateOrGetRenderTargetView();
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
		// Only bother binding it if there are verts to draw
		BindVertexBuffer(vbo);
		Draw(vbo->GetNumVerts(), 0);
	}
}



//----------------------------------------------------------------------------------------------------------------------
void Renderer::ResetRenderingPipelineState()
{
	// Only reset the dirty settings, so changes will be detected and applies on Draw
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
	ID3D11DepthStencilView* dsv = m_depthBuffer->CreateOrGetDepthStencilView();
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
	ID3D11DepthStencilView* view = m_depthBuffer->CreateOrGetDepthStencilView();
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
Font* Renderer::GetDefaultFont() const
{
	return m_defaultFont;
}



//----------------------------------------------------------------------------------------------------------------------
void Renderer::Draw(int vertexCount, int vertexOffset)
{
	UpdateRenderingPipelineState();
	m_deviceContext->Draw(vertexCount, vertexOffset);
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
void Renderer::CreateRenderContext()
{
	UINT deviceFlags = 0;
#ifdef _DEBUG
	deviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif
	
	DXGI_SWAP_CHAIN_DESC swapChainDesc = {};
	swapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	swapChainDesc.BufferDesc.Height = g_window->GetHeight();
	swapChainDesc.BufferDesc.Width = g_window->GetWidth();
	swapChainDesc.Windowed = TRUE;
	swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
	swapChainDesc.Flags = 0;
	swapChainDesc.BufferCount = 2;
	swapChainDesc.OutputWindow = (HWND) g_window->GetHWND();
	swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	swapChainDesc.SampleDesc.Count = 1;

	HRESULT result = D3D11CreateDeviceAndSwapChain(
		nullptr,					
		D3D_DRIVER_TYPE_HARDWARE,	
		nullptr,					
		deviceFlags,
		nullptr,					
		0,							
		D3D11_SDK_VERSION,
		&swapChainDesc,
		&m_swapChain,
		&m_device,
		nullptr,
		&m_deviceContext
	);

	ASSERT_OR_DIE(SUCCEEDED(result), "Failed to create device and swap chain")

	m_backbufferTexture = new Texture();
	m_backbufferTexture->CreateFromSwapChain(m_swapChain);
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
void Renderer::CreateDepthBuffer()
{
	Window* window = g_window;
	m_depthBuffer = Texture::CreateDepthBuffer(window->GetDimensions());
}



//----------------------------------------------------------------------------------------------------------------------
void Renderer::DestroyDepthBuffer()
{
	m_depthBuffer->ReleaseResources();
	delete m_depthBuffer;
	m_depthBuffer = nullptr;
}



//----------------------------------------------------------------------------------------------------------------------
void Renderer::DestroyDepthStencilState()
{
	DX_SAFE_RELEASE(m_depthStencilState)
}



//----------------------------------------------------------------------------------------------------------------------
void Renderer::UpdateRenderingPipelineState(bool force)
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
	
	// Default Blend Mode, move to separate default settings function?
	SetBlendMode(BlendMode::Alpha);
}



//----------------------------------------------------------------------------------------------------------------------
void Renderer::DestroyRenderContext()
{
	m_deviceContext->ClearState();
	m_deviceContext->Flush();
	
	if (m_backbufferTexture)
	{
		m_backbufferTexture->ReleaseResources();
	}
	delete m_backbufferTexture;
	m_backbufferTexture = nullptr;
	
	DX_SAFE_RELEASE(m_swapChain)
	DX_SAFE_RELEASE(m_deviceContext)
	DX_SAFE_RELEASE(m_device)
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




