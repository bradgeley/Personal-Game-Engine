// Bradley Christensen - 2022-2023
#include "D3D11Renderer.h"
#include "D3D11ConstantBuffer.h"
#include "D3D11Internal.h"
#include "D3D11Shader.h"
#include "D3D11Texture.h"
#include "D3D11Utils.h"
#include "D3D11VertexBuffer.h"
#include "D3D11SwapchainRenderTarget.h"
#include "HLSLDefaultShaderSource.h"
#include "HLSLDefaultFontShaderSource.h"
#include "Engine/Core/ErrorUtils.h"
#include "Engine/Core/StringUtils.h"
#include "Engine/Renderer/Font.h"
#include "Engine/Renderer/RenderTarget.h"

#if defined(_DEBUG)
#include <dxgidebug.h>
#endif
#pragma comment(lib, "dxguid.lib")



//----------------------------------------------------------------------------------------------------------------------
D3D11Renderer::D3D11Renderer(RendererConfig const& config) : RendererInterface(config)
{

}



//----------------------------------------------------------------------------------------------------------------------
D3D11Renderer* D3D11Renderer::Get()
{
    return reinterpret_cast<D3D11Renderer*>(g_rendererInterface);
}



//----------------------------------------------------------------------------------------------------------------------
void D3D11Renderer::Present() const
{
	ASSERT_OR_DIE(m_currentRenderTarget, "Present called with no render target bound.");

	// TODO: make so i dont have to reinterpret cast
	D3D11SwapchainRenderTarget* swapchainRT = reinterpret_cast<D3D11SwapchainRenderTarget*>(m_currentRenderTarget);

	// Get the backbuffer texture
	ID3D11Texture2D* swapChainBackBufferTexture = nullptr;
	HRESULT hr = swapchainRT->m_swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), reinterpret_cast<void**>(&swapChainBackBufferTexture));

	ASSERT_OR_DIE(SUCCEEDED(hr), "Swap chain did not have a backbuffer texture.");

	ID3D11DeviceContext* context = D3D11Renderer::Get()->GetDeviceContext();

	// Paint onto the actual backbuffer texture using the texture that we've been rendering to at higher quality (lets us use MSAA)
	context->ResolveSubresource(
		swapChainBackBufferTexture, // actual backbuffer d3d texture
		0,
		reinterpret_cast<D3D11Texture*>(swapchainRT->m_backbufferTexture)->m_textureHandle, // higher sample count texture that we've been rendering to
		0,
		DXGI_FORMAT_R8G8B8A8_UNORM
	);

	DX_SAFE_RELEASE(swapChainBackBufferTexture);

	UINT vsync = m_perUserSettings.m_vsyncEnabled ? 1 : 0;
	swapchainRT->m_swapChain->Present(vsync, 0);
}



//----------------------------------------------------------------------------------------------------------------------
void D3D11Renderer::ClearScreen(Rgba8 const& tint)
{
	float colorAsFloats[4] = {};
	tint.GetAsFloats(&colorAsFloats[0]);
	ID3D11RenderTargetView* const renderTargetView = reinterpret_cast<D3D11Texture*>(m_currentRenderTarget->m_backbufferTexture)->CreateOrGetRenderTargetView();
	m_deviceContext->ClearRenderTargetView(renderTargetView, colorAsFloats);
}



//----------------------------------------------------------------------------------------------------------------------
void D3D11Renderer::ClearDepth(float depth)
{
	ASSERT_OR_DIE(m_currentRenderTarget && m_currentRenderTarget->m_depthBuffer, "Tried to clear null depth buffer.");
	ID3D11DepthStencilView* view = reinterpret_cast<D3D11Texture*>(m_currentRenderTarget->m_depthBuffer)->CreateOrGetDepthStencilView();
	m_deviceContext->ClearDepthStencilView(view, D3D11_CLEAR_DEPTH, depth, 0);
}



//----------------------------------------------------------------------------------------------------------------------
void D3D11Renderer::BindVertexBuffer(VertexBuffer const* vbo) const
{
	UINT stride = (UINT) vbo->GetStride();
	UINT offset = 0;

	m_deviceContext->IASetVertexBuffers(
		0,
		1,
		&reinterpret_cast<D3D11VertexBuffer const*>(vbo)->m_handle,
		&stride,
		&offset
	);

	ID3D11InputLayout* vertexLayout = reinterpret_cast<D3D11Shader*>(m_defaultShader)->CreateOrGetInputLayout();
	m_deviceContext->IASetInputLayout(vertexLayout);
}



//----------------------------------------------------------------------------------------------------------------------
void D3D11Renderer::BindConstantBuffer(ConstantBuffer const* cbo, int slot) const
{
	m_deviceContext->VSSetConstantBuffers(slot, 1, &reinterpret_cast<D3D11ConstantBuffer const*>(cbo)->m_handle);
	m_deviceContext->PSSetConstantBuffers(slot, 1, &reinterpret_cast<D3D11ConstantBuffer const*>(cbo)->m_handle);
}



//----------------------------------------------------------------------------------------------------------------------
Texture* D3D11Renderer::MakeTexture() const
{
	return new D3D11Texture();
}



//----------------------------------------------------------------------------------------------------------------------
Shader* D3D11Renderer::MakeShader(ShaderConfig const& config) const
{
	return new D3D11Shader(config);
}



//----------------------------------------------------------------------------------------------------------------------
ConstantBuffer* D3D11Renderer::MakeConstantBuffer() const
{
	return new D3D11ConstantBuffer();
}



//----------------------------------------------------------------------------------------------------------------------
VertexBuffer* D3D11Renderer::MakeVertexBuffer() const
{
	return new D3D11VertexBuffer();
}



//----------------------------------------------------------------------------------------------------------------------
RenderTarget* D3D11Renderer::MakeSwapchainRenderTarget(void* hwnd, IntVec2 const& resolution) const
{
	D3D11SwapchainRenderTarget* renderTarget = new D3D11SwapchainRenderTarget();
	renderTarget->m_renderDimensions = resolution;
	renderTarget->m_backbufferTexture = new D3D11Texture();
	renderTarget->m_depthBuffer = new D3D11Texture();

	// Create the swap chain
	IDXGIDevice* device;
	m_device->QueryInterface(__uuidof(IDXGIDevice), (void**) &device);

	IDXGIAdapter* adapter;
	device->GetParent(__uuidof(IDXGIAdapter), (void**) &adapter);

	IDXGIFactory* factory;
	adapter->GetParent(__uuidof(IDXGIFactory), (void**) &factory);

	DXGI_SWAP_CHAIN_DESC swapChainDesc = {};
	swapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	swapChainDesc.BufferDesc.Height = resolution.y;
	swapChainDesc.BufferDesc.Width = resolution.x;
	swapChainDesc.BufferDesc.RefreshRate = { 0, 0 };
	swapChainDesc.Windowed = TRUE;
	swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
	swapChainDesc.Flags = 0;
	swapChainDesc.BufferCount = 2;
	swapChainDesc.OutputWindow = (HWND) hwnd;
	swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	swapChainDesc.SampleDesc.Count = 1;

	HRESULT result = factory->CreateSwapChain(m_device, &swapChainDesc, &renderTarget->m_swapChain);
	if (!SUCCEEDED(result))
	{
		delete renderTarget->m_backbufferTexture;
		delete renderTarget->m_depthBuffer;
		delete renderTarget;
		return nullptr;
	}

	DX_SAFE_RELEASE(factory);
	DX_SAFE_RELEASE(adapter);
	DX_SAFE_RELEASE(device);

	reinterpret_cast<D3D11Texture*>(renderTarget->m_backbufferTexture)->InitAsBackbufferTexture(renderTarget->m_swapChain);
	reinterpret_cast<D3D11Texture*>(renderTarget->m_depthBuffer)->InitAsDepthBuffer(renderTarget->m_swapChain);

	return renderTarget;
}



//----------------------------------------------------------------------------------------------------------------------
void D3D11Renderer::ReleaseSwapchainRenderTarget(RenderTarget* renderTarget) const
{
	D3D11SwapchainRenderTarget* scrt = reinterpret_cast<D3D11SwapchainRenderTarget*>(renderTarget);
	DX_SAFE_RELEASE(scrt->m_swapChain);

	scrt->m_backbufferTexture->ReleaseResources();
	delete scrt->m_backbufferTexture;
	scrt->m_backbufferTexture = nullptr;

	scrt->m_depthBuffer->ReleaseResources();
	delete scrt->m_depthBuffer;
	scrt->m_depthBuffer = nullptr;
}



//----------------------------------------------------------------------------------------------------------------------
ID3D11Device* D3D11Renderer::GetDevice() const
{
    return m_device;
}



//----------------------------------------------------------------------------------------------------------------------
ID3D11DeviceContext* D3D11Renderer::GetDeviceContext() const
{
    return m_deviceContext;
}



//----------------------------------------------------------------------------------------------------------------------
void D3D11Renderer::BindRenderTarget(RenderTarget* renderTarget)
{
	ASSERT_OR_DIE(renderTarget && renderTarget->m_backbufferTexture && renderTarget->m_depthBuffer, "Null render target.");
	m_currentRenderTarget = renderTarget;

	ID3D11DepthStencilView* dsv = reinterpret_cast<D3D11Texture*>(m_currentRenderTarget->m_depthBuffer)->CreateOrGetDepthStencilView();
	ID3D11RenderTargetView* rtv = reinterpret_cast<D3D11Texture*>(m_currentRenderTarget->m_backbufferTexture)->CreateOrGetRenderTargetView();
	m_deviceContext->OMSetRenderTargets(1, &rtv, dsv);

	// Set viewport
	IntVec2 renderResolution = m_currentRenderTarget->m_renderDimensions;

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
void D3D11Renderer::ResizeSwapChainRenderTarget(D3D11SwapchainRenderTarget* renderTarget)
{
	renderTarget->m_backbufferTexture->ReleaseResources();
	renderTarget->m_depthBuffer->ReleaseResources();

	DXGI_SWAP_CHAIN_DESC desc = {};
	renderTarget->m_swapChain->GetDesc(&desc);

	IntVec2 renderResolution = renderTarget->m_renderDimensions;

	HRESULT hr = renderTarget->m_swapChain->ResizeBuffers(
		desc.BufferCount,
		renderResolution.x,
		renderResolution.y,
		desc.BufferDesc.Format,
		desc.Flags
	);

	ASSERT_OR_DIE(SUCCEEDED(hr), "Failed to resize buffers after window mode change.");

	reinterpret_cast<D3D11Texture*>(renderTarget->m_backbufferTexture)->InitAsBackbufferTexture(renderTarget->m_swapChain);
	reinterpret_cast<D3D11Texture*>(renderTarget->m_depthBuffer)->InitAsDepthBuffer(renderTarget->m_swapChain);
}



//----------------------------------------------------------------------------------------------------------------------
MSAASettings D3D11Renderer::GetMaxSupportedMSAASettings(DXGI_FORMAT format)
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
void D3D11Renderer::Draw(int vertexCount, int vertexOffset)
{
	RendererInterface::Draw(vertexCount, vertexOffset);
	m_deviceContext->Draw(vertexCount, vertexOffset);
}



//----------------------------------------------------------------------------------------------------------------------
void D3D11Renderer::RasterizerStateUpdated()
{
	DX_SAFE_RELEASE(m_rasterizerState);
	CreateRasterizerState();
	m_deviceContext->RSSetState(m_rasterizerState);
}



//----------------------------------------------------------------------------------------------------------------------
void D3D11Renderer::DepthStencilStateUpdated()
{
	DX_SAFE_RELEASE(m_depthStencilState);

	D3D11_DEPTH_STENCIL_DESC desc;
	desc.DepthEnable = TRUE;
	desc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
	desc.DepthFunc = D3D11_COMPARISON_ALWAYS;
	desc.StencilEnable = FALSE;

	m_device->CreateDepthStencilState(&desc, &m_depthStencilState);
	m_deviceContext->OMSetDepthStencilState(m_depthStencilState, 0);
}



//----------------------------------------------------------------------------------------------------------------------
void D3D11Renderer::BlendModeUpdated()
{
	ASSERT_OR_DIE(m_blendStateByMode[(int) m_settings.m_blendMode], "SetBlendMode Failed due to invalid blend state.");
	float constexpr blendConstants[4] = { 0.f, 0.f, 0.f, 0.f };
	m_deviceContext->OMSetBlendState(m_blendStateByMode[(int) m_settings.m_blendMode], blendConstants, 0xffffffff);
}



//----------------------------------------------------------------------------------------------------------------------
void D3D11Renderer::SamplerStateUpdated()
{
	DX_SAFE_RELEASE(m_samplerState);

	m_samplerState = CreateSamplerState(m_settings.m_samplerFilter, m_settings.m_samplerAddressMode);
	ASSERT_OR_DIE(m_samplerState, "Failed to create or get sampler state");

	// todo: support multiple samplers at a time
	m_deviceContext->PSSetSamplers(0, 1, &m_samplerState);
}



//----------------------------------------------------------------------------------------------------------------------
void D3D11Renderer::BoundTextureUpdated()
{
	ASSERT_OR_DIE(m_settings.m_texture, "Tried to update null bound texture.");
	ID3D11ShaderResourceView* srv = reinterpret_cast<D3D11Texture*>(m_settings.m_texture)->CreateOrGetShaderResourceView();

	// todo: support multiple textures bound at the same time
	m_deviceContext->PSSetShaderResources(0, 1, &srv);
}



//----------------------------------------------------------------------------------------------------------------------
void D3D11Renderer::BoundShaderUpdated()
{
	ASSERT_OR_DIE(m_settings.m_shader, "Tried to update null bound shader.");

	ID3D11PixelShader* pixelShader = reinterpret_cast<D3D11Shader*>(m_settings.m_shader)->m_pixelShader;
	ASSERT_OR_DIE(pixelShader, "Shader has no valid pixel shader.");
	ID3D11VertexShader* vertexShader = reinterpret_cast<D3D11Shader*>(m_settings.m_shader)->m_vertexShader;
	ASSERT_OR_DIE(vertexShader, "Shader has no valid vertex shader.");

	m_deviceContext->VSSetShader(vertexShader, nullptr, 0);
	m_deviceContext->PSSetShader(pixelShader, nullptr, 0);
	m_deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST); // todo: put into shader class
}



//----------------------------------------------------------------------------------------------------------------------
void D3D11Renderer::CreateDebugLayer()
{
#if defined(_DEBUG)
	HMODULE modHandle = ::LoadLibraryA("Dxgidebug.dll");
	ASSERT_OR_DIE(modHandle, "Failed to load Dxgidebug.dll");

	typedef HRESULT(WINAPI* GetDebugModuleFunc)(REFIID, void**);
	GetDebugModuleFunc getModuleFunc = (GetDebugModuleFunc) ::GetProcAddress(modHandle, "DXGIGetDebugInterface");
	ASSERT_OR_DIE(getModuleFunc, "Failed to acquire DXGIGetDebugInterface");

	HRESULT result = getModuleFunc(__uuidof(IDXGIDebug), (void**) &m_debug);
	ASSERT_OR_DIE(SUCCEEDED(result), "getModuleFunc Failed");

	m_debugModule = modHandle;
#endif
}



//----------------------------------------------------------------------------------------------------------------------
void D3D11Renderer::CreateDevice()
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
void D3D11Renderer::CreateBlendStates()
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
void D3D11Renderer::CreateRasterizerState()
{
	DX_SAFE_RELEASE(m_rasterizerState);

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
	ASSERT_OR_DIE(m_rasterizerState, "Failed to create rasterizer state");
}



//----------------------------------------------------------------------------------------------------------------------
void D3D11Renderer::CreateDefaultShader()
{
	ShaderConfig defaultShaderConfig;
	defaultShaderConfig.m_name = "DefaultShader";
	m_defaultShader = new D3D11Shader(defaultShaderConfig);
	reinterpret_cast<D3D11Shader*>(m_defaultShader)->CreateFromSource(s_HLSLDefaultShaderSource);
}



//----------------------------------------------------------------------------------------------------------------------
void RendererInterface::CreateDefaultTexture()
{
	m_defaultTexture = MakeTexture();
	m_defaultTexture->CreateUniformTexture(IntVec2(1, 1), Rgba8::White);
}



//----------------------------------------------------------------------------------------------------------------------
void D3D11Renderer::CreateDefaultFont()
{
	m_defaultFont = new Font();
	m_defaultFont->LoadFNT("Data/Fonts/Gypsy.fnt");
	ShaderConfig config;
	config.m_name = "Default Font Shader";
	m_defaultFont->m_shader = MakeShader(config);
	m_defaultFont->m_shader->CreateFromSource(s_HLSLDefaultFontShaderSource);
}



//----------------------------------------------------------------------------------------------------------------------
ID3D11BlendState* D3D11Renderer::CreateBlendState(D3D11_BLEND srcFactor, D3D11_BLEND dstFactor, D3D11_BLEND_OP op)
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

	ASSERT_OR_DIE(SUCCEEDED(result), "Failed to create blend state");

	return blendState;
}



//----------------------------------------------------------------------------------------------------------------------
ID3D11SamplerState* D3D11Renderer::CreateSamplerState(SamplerFilter filter, SamplerAddressMode addressMode)
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
	ASSERT_OR_DIE(SUCCEEDED(result), "Failed to create sampler state.");
	return samplerState;
}



//----------------------------------------------------------------------------------------------------------------------
void D3D11Renderer::DestroyDebugLayer()
{
	ReportLiveObjects();

	#if defined(_DEBUG)
		if (m_debugModule)
		{
			DX_SAFE_RELEASE(m_debug);
			::FreeLibrary((HMODULE) m_debugModule);
			m_debugModule = nullptr;
		}
	#endif
}



//----------------------------------------------------------------------------------------------------------------------
void D3D11Renderer::DestroyDevice()
{
	m_deviceContext->ClearState();
	m_deviceContext->Flush();

	DX_SAFE_RELEASE(m_deviceContext);
	DX_SAFE_RELEASE(m_device);
}



//----------------------------------------------------------------------------------------------------------------------
void D3D11Renderer::DestroyBlendStates()
{
	DX_SAFE_RELEASE(m_blendStateByMode[(int) BlendMode::Opaque]);
	DX_SAFE_RELEASE(m_blendStateByMode[(int) BlendMode::Additive]);
	DX_SAFE_RELEASE(m_blendStateByMode[(int) BlendMode::Alpha]);
}



//----------------------------------------------------------------------------------------------------------------------
void D3D11Renderer::DestroyDepthStencilState()
{
	DX_SAFE_RELEASE(m_depthStencilState)
}



//----------------------------------------------------------------------------------------------------------------------
void D3D11Renderer::DestroySamplerStates()
{
	DX_SAFE_RELEASE(m_samplerState)
}



//----------------------------------------------------------------------------------------------------------------------
void D3D11Renderer::DestroyRasterizerState()
{
	DX_SAFE_RELEASE(m_rasterizerState)
}



//----------------------------------------------------------------------------------------------------------------------
void D3D11Renderer::ReportLiveObjects()
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
bool D3D11Renderer::ToggleMSAA(NamedProperties&)
{
	m_perUserSettings.m_msaaEnabled = !m_perUserSettings.m_msaaEnabled;

	// Recreate the depth buffer and backbuffer textures
	//for (auto& wrcPair : m_windowRenderContexts)
	//{
	//	WindowRenderContext& wrc = wrcPair.second;
	//	wrc.m_backbufferTexture->ReleaseResources();
	//	wrc.m_depthBuffer->ReleaseResources();
	//
	//	D3D11Texture* backbufferTexture = new D3D11Texture();
	//	backbufferTexture->InitAsBackbufferTexture(wrc.m_swapChain);
	//	wrc.m_backbufferTexture = backbufferTexture;
	//
	//	D3D11Texture* depthBuffer = new D3D11Texture();
	//	depthBuffer->InitAsDepthBuffer(wrc.m_swapChain);
	//	wrc.m_depthBuffer = depthBuffer;
	//}

	return false;
}