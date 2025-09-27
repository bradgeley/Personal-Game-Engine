// Bradley Christensen - 2022-2025
#include "Game/Framework/EngineBuildPreferences.h"

#if defined(RENDERER_D3D11)

#include "D3D11Renderer.h"
#include "D3D11ConstantBuffer.h"
#include "D3D11Internal.h"
#include "D3D11Shader.h"
#include "D3D11Texture.h"
#include "D3D11GPUBuffer.h"
#include "D3D11Utils.h"
#include "D3D11VertexBuffer.h"
#include "D3D11InstanceBuffer.h"
#include "D3D11Swapchain.h"
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
D3D11_FILTER GetD3D11SamplerFilter(SamplerFilter filter, float& out_maxAnisotropy);
D3D11_TEXTURE_ADDRESS_MODE GetD3D11SamplerAddressMode(SamplerAddressMode addressMode);
D3D11_CULL_MODE GetD3D11CullMode(CullMode cullMode);
D3D11_FILL_MODE GetD3D11FillMode(FillMode fillMode);



//----------------------------------------------------------------------------------------------------------------------
D3D11_FILTER GetD3D11SamplerFilter(SamplerFilter filter, float& out_maxAnisotropy)
{
	out_maxAnisotropy = 0.f;
	switch (filter)
	{
	case SamplerFilter::Point:
		return D3D11_FILTER_MIN_MAG_MIP_POINT;

	case SamplerFilter::Bilinear:
		return D3D11_FILTER_MIN_MAG_LINEAR_MIP_POINT;

	case SamplerFilter::Trilinear:
		return D3D11_FILTER_MIN_MAG_MIP_LINEAR;

	case SamplerFilter::Anisotropic_2:
		out_maxAnisotropy = 2.f;
		return D3D11_FILTER_ANISOTROPIC;

	case SamplerFilter::Anisotropic_4:
		out_maxAnisotropy = 4.f;
		return D3D11_FILTER_ANISOTROPIC;

	case SamplerFilter::Anisotropic_8:
		out_maxAnisotropy = 8.f;
		return D3D11_FILTER_ANISOTROPIC;

	case SamplerFilter::Anisotropic_16:
		out_maxAnisotropy = 16.f;
		return D3D11_FILTER_ANISOTROPIC;

	default:
		break;
	}
	ERROR_AND_DIE("SamplerFilter not supported.")
}



//----------------------------------------------------------------------------------------------------------------------
D3D11_TEXTURE_ADDRESS_MODE GetD3D11SamplerAddressMode(SamplerAddressMode addressMode)
{
	switch (addressMode)
	{
	case SamplerAddressMode::Wrap:
		return D3D11_TEXTURE_ADDRESS_WRAP;
	default:
		return D3D11_TEXTURE_ADDRESS_CLAMP;
	}
}



//----------------------------------------------------------------------------------------------------------------------
D3D11_CULL_MODE GetD3D11CullMode(CullMode cullMode)
{
	switch (cullMode)
	{
	case CullMode::Front: {}
						return D3D11_CULL_FRONT;
	case CullMode::Back:
		return D3D11_CULL_BACK;
	default:
		return D3D11_CULL_NONE;
	}
}



//----------------------------------------------------------------------------------------------------------------------
D3D11_FILL_MODE GetD3D11FillMode(FillMode fillMode)
{
	switch (fillMode)
	{
	case FillMode::Wireframe:
		return D3D11_FILL_WIREFRAME;
	default:
		return D3D11_FILL_SOLID;
	}
}




//----------------------------------------------------------------------------------------------------------------------
D3D11Renderer::D3D11Renderer(RendererConfig const& config) : Renderer(config)
{

}



//----------------------------------------------------------------------------------------------------------------------
D3D11Renderer* D3D11Renderer::Get()
{
    return dynamic_cast<D3D11Renderer*>(g_renderer);
}



//----------------------------------------------------------------------------------------------------------------------
void D3D11Renderer::Present() const
{
	auto it = m_renderTargets.find(m_currentRenderTarget);
	ASSERT_OR_DIE(it != m_renderTargets.end(), "Present called with no render target bound.");

	RenderTarget* renderTarget = it->second;
	renderTarget->Present();
}



//----------------------------------------------------------------------------------------------------------------------
void D3D11Renderer::ClearScreen(Rgba8 const& tint)
{
	RenderTarget* renderTarget = m_renderTargets.at(m_currentRenderTarget);
	ASSERT_OR_DIE(renderTarget, "Tried to clear null render target.");

	float colorAsFloats[4] = {};
	tint.GetAsFloats(&colorAsFloats[0]);

	D3D11Texture* backbuffer = dynamic_cast<D3D11Texture*>(GetTexture(renderTarget->m_backbufferTexture));
	ASSERT_OR_DIE(backbuffer, "Backbuffer texture is null.");

	ID3D11RenderTargetView* const renderTargetView = backbuffer->CreateOrGetRenderTargetView();
	m_deviceContext->ClearRenderTargetView(renderTargetView, colorAsFloats);
}



//----------------------------------------------------------------------------------------------------------------------
void D3D11Renderer::ClearDepth(float depth)
{
	RenderTarget* renderTarget = m_renderTargets.at(m_currentRenderTarget);
	ASSERT_OR_DIE(renderTarget && renderTarget->m_depthBuffer, "Tried to clear null depth buffer.");

	D3D11Texture* depthBuffer = dynamic_cast<D3D11Texture*>(GetTexture(renderTarget->m_depthBuffer));
	ASSERT_OR_DIE(depthBuffer, "Depth buffer texture is null.");

	ID3D11DepthStencilView* view = depthBuffer->CreateOrGetDepthStencilView();
	m_deviceContext->ClearDepthStencilView(view, D3D11_CLEAR_DEPTH, depth, 0);
}



//----------------------------------------------------------------------------------------------------------------------
void D3D11Renderer::BindVertexBuffer(VertexBufferID id) const
{
	VertexBuffer* vbo = GetVertexBuffer(id);
	ASSERT_OR_DIE(vbo, "Binding invalid vertex buffer");

	BindVertexBuffer(*vbo);
}



//----------------------------------------------------------------------------------------------------------------------
void D3D11Renderer::BindVertexBuffer(VertexBuffer& vbo) const
{
	vbo.UpdateGPUBuffer();

	D3D11VertexBuffer* d3dvbo = dynamic_cast<D3D11VertexBuffer*>(&vbo);
	ASSERT_OR_DIE(d3dvbo, "Trying to draw non-d3d vbo.")

	D3D11GPUBuffer* gpuBuffer = dynamic_cast<D3D11GPUBuffer*>(d3dvbo->m_gpuBuffer);
	ASSERT_OR_DIE(gpuBuffer, "Trying to draw non-d3d gpu buffer.")

	UINT stride = (UINT) vbo.GetStride();
	UINT offset = 0;

	m_deviceContext->IASetVertexBuffers(
		0,
		1,
		&gpuBuffer->m_handle,
		&stride,
		&offset
	);
}



//----------------------------------------------------------------------------------------------------------------------
void D3D11Renderer::BindInstanceBuffer(InstanceBufferID ibo) const
{
	InstanceBuffer* ibuffer = GetInstanceBuffer(ibo);
	ASSERT_OR_DIE(ibuffer, "Binding invalid vertex buffer");

	BindInstanceBuffer(*ibuffer);
}



//----------------------------------------------------------------------------------------------------------------------
void D3D11Renderer::BindInstanceBuffer(InstanceBuffer& ibo) const
{
	ibo.UpdateGPUBuffer();

	D3D11InstanceBuffer* d3dibo = dynamic_cast<D3D11InstanceBuffer*>(&ibo);
	ASSERT_OR_DIE(d3dibo, "Trying to draw null or non-d3d ibo.");

	D3D11GPUBuffer* gpuBuffer = dynamic_cast<D3D11GPUBuffer*>(d3dibo->m_gpuBuffer);
	ASSERT_OR_DIE(gpuBuffer, "Trying to draw non-d3d gpu buffer.");

	UINT stride = (UINT) ibo.GetStride();
	UINT offset = 0;

	m_deviceContext->IASetVertexBuffers(
		1,
		1,
		&gpuBuffer->m_handle,
		&stride,
		&offset
	);
}



//----------------------------------------------------------------------------------------------------------------------
void D3D11Renderer::BindConstantBuffer(ConstantBufferID id, int slot) const
{
	D3D11ConstantBuffer* cbo = dynamic_cast<D3D11ConstantBuffer*>(GetConstantBuffer(id));
	ASSERT_OR_DIE(cbo, "Binding invalid constant buffer");

	D3D11GPUBuffer* gpuBuffer = dynamic_cast<D3D11GPUBuffer*>(cbo->m_gpuBuffer);
	ASSERT_OR_DIE(gpuBuffer, "Trying to draw non-d3d gpu buffer.")

	gpuBuffer->UpdateGPUBuffer();

	m_deviceContext->VSSetConstantBuffers(slot, 1, &gpuBuffer->m_handle);
	m_deviceContext->PSSetConstantBuffers(slot, 1, &gpuBuffer->m_handle);
}



//----------------------------------------------------------------------------------------------------------------------
TextureID D3D11Renderer::MakeTexture()
{
	auto texResult = new D3D11Texture();
	std::unique_lock lock(m_texturesMutex);
	TextureID texID = RequestTextureID();
	m_textures[texID] = texResult;
	return texID;
}



//----------------------------------------------------------------------------------------------------------------------
ShaderID D3D11Renderer::MakeShader(ShaderConfig const& config)
{
	auto shaderResult = new D3D11Shader(config);
	std::unique_lock lock(m_shadersMutex);
	ShaderID shaderID = RequestShaderID();
	m_shaders[shaderID] = shaderResult;
	return shaderID;
}



//----------------------------------------------------------------------------------------------------------------------
ConstantBufferID D3D11Renderer::MakeConstantBuffer(size_t initialSize)
{
	auto cbResult = new D3D11ConstantBuffer();
	cbResult->Initialize(initialSize);
	std::unique_lock lock(m_constantBuffersMutex);
	ConstantBufferID cbID = RequestConstantBufferID();
	m_constantBuffers[cbID] = cbResult;
	return cbID;

}



//----------------------------------------------------------------------------------------------------------------------
VertexBufferID D3D11Renderer::MakeVertexBuffer()
{
	auto vbResult = new D3D11VertexBuffer();
	std::unique_lock lock(m_vertexBuffersMutex);
	VertexBufferID vbID = RequestVertexBufferID();
	m_vertexBuffers[vbID] = vbResult;
	return vbID;
}



//----------------------------------------------------------------------------------------------------------------------
InstanceBufferID D3D11Renderer::MakeInstanceBuffer()
{
	auto ibResult = new D3D11InstanceBuffer();
	std::unique_lock lock(m_instanceBuffersMutex);
	InstanceBufferID ibID = RequestInstanceBufferID();
	m_instanceBuffers[ibID] = ibResult;
	return ibID;
}



//----------------------------------------------------------------------------------------------------------------------
SwapchainID D3D11Renderer::MakeSwapchain()
{
	auto scResult = new D3D11Swapchain();
	std::unique_lock lock(m_swapchainsMutex);
	SwapchainID scID = RequestSwapchainID();
	m_swapchains[scID] = scResult;
	return scID;
}



//----------------------------------------------------------------------------------------------------------------------
RenderTargetID D3D11Renderer::MakeSwapchainRenderTarget(void* hwnd, IntVec2 const& resolution)
{
	RenderTarget* renderTarget = new RenderTarget();
	renderTarget->m_renderDimensions = resolution;
	renderTarget->m_backbufferTexture = MakeTexture();
	renderTarget->m_depthBuffer = MakeTexture();
	renderTarget->m_swapchain = MakeSwapchain();

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

	D3D11Swapchain* d3d11Swapchain = dynamic_cast<D3D11Swapchain*>(GetSwapchain(renderTarget->m_swapchain));
	HRESULT result = factory->CreateSwapChain(m_device, &swapChainDesc, &d3d11Swapchain->m_swapChain);

	DX_SAFE_RELEASE(factory);
	DX_SAFE_RELEASE(adapter);
	DX_SAFE_RELEASE(device);

	if (!SUCCEEDED(result))
	{
		ReleaseTexture(renderTarget->m_backbufferTexture);
		ReleaseTexture(renderTarget->m_depthBuffer);
		ReleaseSwapchain(renderTarget->m_swapchain);
		delete renderTarget;
		return RendererUtils::InvalidID;
	}

	#if defined(_DEBUG)
		std::string swapchainString = "Swapchain";
		d3d11Swapchain->m_swapChain->SetPrivateData(WKPDID_D3DDebugObjectName, (int) swapchainString.size(), swapchainString.data());
	#endif

	D3D11Texture* backBuffer = dynamic_cast<D3D11Texture*>(GetTexture(renderTarget->m_backbufferTexture));
	D3D11Texture* depthBuffer = dynamic_cast<D3D11Texture*>(GetTexture(renderTarget->m_depthBuffer));

	ASSERT_OR_DIE(backBuffer, "Invalid backbuffer");
	ASSERT_OR_DIE(depthBuffer, "Invalid depth buffer");

	bool backBufferSuccess = backBuffer->InitAsBackbufferTexture(d3d11Swapchain->m_swapChain);
	bool depthBufferSuccess = depthBuffer->InitAsDepthBuffer(d3d11Swapchain->m_swapChain);
	depthBuffer->CreateOrGetDepthStencilView();

	ASSERT_OR_DIE(backBufferSuccess, "Backbuffer failed to init.");
	ASSERT_OR_DIE(depthBufferSuccess, "Depth buffer failed to init");

	std::unique_lock lock(m_renderTargetsMutex);
	RenderTargetID newID = RequestRenderTargetID();
	m_renderTargets[newID] = renderTarget;
	return newID;
}



//----------------------------------------------------------------------------------------------------------------------
GPUBuffer* D3D11Renderer::MakeGPUBuffer(GpuBufferConfig const& config)
{
	return new D3D11GPUBuffer(config);
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
void D3D11Renderer::BindRenderTarget(RenderTargetID renderTargetID)
{
	m_currentRenderTarget = renderTargetID;
	RenderTarget* renderTarget = GetRenderTarget(renderTargetID);

	ASSERT_OR_DIE(renderTarget && renderTarget->m_backbufferTexture && renderTarget->m_depthBuffer, "Null render target.");

	D3D11Texture* backbuffer = dynamic_cast<D3D11Texture*>(GetTexture(renderTarget->m_backbufferTexture));
	D3D11Texture* depthBuffer = dynamic_cast<D3D11Texture*>(GetTexture(renderTarget->m_depthBuffer));

	ASSERT_OR_DIE(backbuffer, "Invalid backbuffer");
	ASSERT_OR_DIE(depthBuffer, "Invalid depth buffer");

	ID3D11DepthStencilView* dsv = depthBuffer->CreateOrGetDepthStencilView();
	ID3D11RenderTargetView* rtv = backbuffer->CreateOrGetRenderTargetView();
	m_deviceContext->OMSetRenderTargets(1, &rtv, dsv);

	// Set viewport
	IntVec2 renderResolution = renderTarget->m_renderDimensions;

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
void D3D11Renderer::ResizeSwapChainRenderTarget(RenderTargetID renderTargetID, IntVec2 const& newSize)
{
	RenderTarget* renderTarget = m_renderTargets[renderTargetID];
	ASSERT_OR_DIE(renderTarget && renderTarget->m_backbufferTexture && renderTarget->m_depthBuffer, "Null render target.");

	D3D11Texture* backBuffer = dynamic_cast<D3D11Texture*>(GetTexture(renderTarget->m_backbufferTexture));
	D3D11Texture* depthBuffer = dynamic_cast<D3D11Texture*>(GetTexture(renderTarget->m_depthBuffer));
	D3D11Swapchain* swapchain = dynamic_cast<D3D11Swapchain*>(GetSwapchain(renderTarget->m_swapchain));

	ASSERT_OR_DIE(backBuffer, "Render target has an invalid backbuffer");
	ASSERT_OR_DIE(depthBuffer, "Render target has an invalid depth buffer");
	ASSERT_OR_DIE(swapchain, "Render target must have a swapchain to call ResizeSwapChainRenderTarget.");

	backBuffer->ReleaseResources();
	depthBuffer->ReleaseResources();

	DXGI_SWAP_CHAIN_DESC desc = {};
	swapchain->m_swapChain->GetDesc(&desc);

	renderTarget->m_renderDimensions = newSize;

	HRESULT hr = swapchain->m_swapChain->ResizeBuffers(
		desc.BufferCount,
		renderTarget->m_renderDimensions.x,
		renderTarget->m_renderDimensions.y,
		desc.BufferDesc.Format,
		desc.Flags
	);

	ASSERT_OR_DIE(SUCCEEDED(hr), "Failed to resize buffers after window mode change.");

	backBuffer->InitAsBackbufferTexture(swapchain->m_swapChain);
	depthBuffer->InitAsDepthBuffer(swapchain->m_swapChain);
}



//----------------------------------------------------------------------------------------------------------------------
bool D3D11Renderer::SetFullscreenState(RenderTargetID renderTargetID, bool fullscreen)
{
	RenderTarget* rt = GetRenderTarget(renderTargetID);
	ASSERT_OR_DIE(rt, "Invalid renderTarget");

	D3D11Swapchain* swapchain = dynamic_cast<D3D11Swapchain*>(GetSwapchain(rt->m_swapchain));
	ASSERT_OR_DIE(swapchain, "Invalid swapchain");

	HRESULT hr = swapchain->m_swapChain->SetFullscreenState((BOOL) fullscreen, nullptr);
	return SUCCEEDED(hr);
}



//----------------------------------------------------------------------------------------------------------------------
MSAASettings D3D11Renderer::GetMaxSupportedMSAASettings(DXGI_FORMAT format)
{
	MSAASettings result;
	if (!m_userSettings.m_msaaEnabled)
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
void D3D11Renderer::Draw(int vertexCount, int vertexOffset /*= 0*/)
{
	Renderer::Draw(vertexCount, vertexOffset);
	m_deviceContext->Draw(vertexCount, vertexOffset);
}



//----------------------------------------------------------------------------------------------------------------------
void D3D11Renderer::DrawInstanced(int vertexCount, int instanceCount, int vertexOffset /*= 0*/, int instanceOffset /*= 0*/)
{
	Renderer::DrawInstanced(vertexCount, instanceCount, vertexOffset, instanceOffset);
	m_deviceContext->DrawInstanced(vertexCount, instanceCount, vertexOffset, instanceOffset);
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
	desc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	desc.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;
	desc.StencilEnable = FALSE;

	HRESULT hr = m_device->CreateDepthStencilState(&desc, &m_depthStencilState);
	ASSERT_OR_DIE(SUCCEEDED(hr), "Failed to create depth stencil state.")

	#ifdef _DEBUG
		std::string depthStencilStateName = StringUtils::StringF("Depth Stencil State");
		m_depthStencilState->SetPrivateData(WKPDID_D3DDebugObjectName, (int) depthStencilStateName.size(), depthStencilStateName.data());
	#endif

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
void D3D11Renderer::BoundTextureUpdated(int slot /*= 0*/)
{
	D3D11Texture* boundTexture = dynamic_cast<D3D11Texture*>(GetBoundTexture(slot));

	ID3D11ShaderResourceView* srv = nullptr;
	if (boundTexture)
	{
		srv = boundTexture->CreateOrGetShaderResourceView();
	}

	// Will bind the texture if not null, otherwise unbinds with a null srv
	m_deviceContext->PSSetShaderResources(slot, 1, &srv);
}



//----------------------------------------------------------------------------------------------------------------------
void D3D11Renderer::BoundShaderUpdated()
{
	D3D11Shader* boundShader = dynamic_cast<D3D11Shader*>(GetBoundShader());
	ASSERT_OR_DIE(boundShader, "Tried to update null or non-d3d shader.");

	ID3D11PixelShader* pixelShader = boundShader->m_pixelShader;
	ASSERT_OR_DIE(pixelShader, "Shader has no valid pixel shader.");
	ID3D11VertexShader* vertexShader = boundShader->m_vertexShader;
	ASSERT_OR_DIE(vertexShader, "Shader has no valid vertex shader.");

	m_deviceContext->VSSetShader(vertexShader, nullptr, 0);
	m_deviceContext->PSSetShader(pixelShader, nullptr, 0);

	ID3D11InputLayout* vertexLayout = boundShader->GetD3D11InputLayout();
	ASSERT_OR_DIE(vertexLayout, "Shader has no valid input layout.");
	m_deviceContext->IASetInputLayout(vertexLayout);
	m_deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST); // todo: put into shader class?
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
	m_blendStateByMode[(int) BlendMode::Opaque]		= CreateBlendState(BlendMode::Opaque);		// D3D11_BLEND_ONE, D3D11_BLEND_ZERO, D3D11_BLEND_OP_ADD);
	m_blendStateByMode[(int) BlendMode::Alpha]		= CreateBlendState(BlendMode::Alpha);		// D3D11_BLEND_SRC_ALPHA, D3D11_BLEND_INV_SRC_ALPHA, D3D11_BLEND_OP_ADD);
	m_blendStateByMode[(int) BlendMode::Additive]	= CreateBlendState(BlendMode::Additive);	// D3D11_BLEND_ONE, D3D11_BLEND_ONE, D3D11_BLEND_OP_ADD);

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

	HRESULT hr = m_device->CreateRasterizerState(&rasterizerDesc, &m_rasterizerState);
	ASSERT_OR_DIE(SUCCEEDED(hr), "Failed to create rasterizer state");
	#ifdef _DEBUG
		std::string rasterizerName = StringUtils::StringF("Rasterizer State");
		m_rasterizerState->SetPrivateData(WKPDID_D3DDebugObjectName, (int) rasterizerName.size(), rasterizerName.data());
	#endif
}



//----------------------------------------------------------------------------------------------------------------------
void D3D11Renderer::CreateDefaultShader()
{
	ShaderConfig defaultShaderConfig;
	defaultShaderConfig.m_name = "DefaultShader";
	defaultShaderConfig.m_inputLayout = *Vertex_PCU::GetInputLayout();

	m_defaultShader = MakeShader(defaultShaderConfig);

	D3D11Shader* defaultShader = dynamic_cast<D3D11Shader*>(GetDefaultShader());
	ASSERT_OR_DIE(defaultShader, "Invalid default shader.");

	defaultShader->FullCompileFromSource(s_HLSLDefaultShaderSource);
	defaultShader->GetD3D11InputLayout();
}



//----------------------------------------------------------------------------------------------------------------------
void D3D11Renderer::CreateDefaultFont()
{
	m_defaultFont = MakeFont();

	Font* font = GetFont(m_defaultFont);
	ASSERT_OR_DIE(font, "Invalid default font.");

	font->LoadFNT("Data/Fonts/Gypsy.fnt");

	ShaderConfig config;
	config.m_inputLayout = *Vertex_PCU::GetInputLayout();
	config.m_name = "Default Font Shader";
	font->m_shader = MakeShader(config);

	Shader* shader = GetShader(font->m_shader);
	ASSERT_OR_DIE(shader, "Invalid default font shader.");

	shader->FullCompileFromSource(s_HLSLDefaultFontShaderSource);
}



//----------------------------------------------------------------------------------------------------------------------
// Opaque: D3D11_BLEND_ONE, D3D11_BLEND_ZERO, D3D11_BLEND_OP_ADD);
// Alpha: D3D11_BLEND_SRC_ALPHA, D3D11_BLEND_INV_SRC_ALPHA, D3D11_BLEND_OP_ADD);
// Additive: D3D11_BLEND_ONE, D3D11_BLEND_ONE, D3D11_BLEND_OP_ADD);
//
ID3D11BlendState* D3D11Renderer::CreateBlendState(BlendMode blendMode)
{
	auto srcFactor = blendMode == BlendMode::Opaque ? D3D11_BLEND_ONE :
					 blendMode == BlendMode::Alpha ? D3D11_BLEND_SRC_ALPHA :
					 blendMode == BlendMode::Additive ? D3D11_BLEND_ONE : D3D11_BLEND_ONE;

	auto dstFactor = blendMode == BlendMode::Opaque ? D3D11_BLEND_ZERO :
					 blendMode == BlendMode::Alpha ? D3D11_BLEND_INV_SRC_ALPHA :
					 blendMode == BlendMode::Additive ? D3D11_BLEND_ONE : D3D11_BLEND_ZERO;

	auto op = blendMode == BlendMode::Opaque ? D3D11_BLEND_OP_ADD :
			  blendMode == BlendMode::Alpha ? D3D11_BLEND_OP_ADD :
			  blendMode == BlendMode::Additive ? D3D11_BLEND_OP_ADD : D3D11_BLEND_OP_ADD;

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
			DXGI_DEBUG_RLO_FLAGS flags = (DXGI_DEBUG_RLO_FLAGS) (DXGI_DEBUG_RLO_DETAIL);
			m_debug->ReportLiveObjects(DXGI_DEBUG_ALL, flags);
		}
	#endif
}



//----------------------------------------------------------------------------------------------------------------------
bool D3D11Renderer::ToggleMSAA(NamedProperties&)
{
	m_userSettings.m_msaaEnabled = !m_userSettings.m_msaaEnabled;

	// Recreate the depth buffer and backbuffer textures
	for (auto& rt : m_renderTargets)
	{
		ResizeSwapChainRenderTarget(rt.first, rt.second->m_renderDimensions);
	}

	return false;
}

#endif // RENDERER_D3D11