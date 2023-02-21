// Bradley Christensen - 2022
#include "Renderer.h"
#include "RendererInternal.h"
#include "Texture.h"
#include "Window.h"
#include "Engine/Core/ErrorUtils.h"



//----------------------------------------------------------------------------------------------------------------------
// THE Renderer
//
Renderer* g_renderer = nullptr;



Renderer::Renderer(RendererConfig const& config) : m_config(config)
{
    
}



void Renderer::Startup()
{
	CreateRenderContext();
	//CreateDefaultShader();
	CreateBlendStates();
}



void Renderer::BeginFrame()
{
    
}



void Renderer::BindRenderTarget(Texture* backbuffer)
{
	//ID3D11DepthStencilView* dsv = m_defaultDepthStencil->GetOrCreateDepthStencilView( this );
	ID3D11RenderTargetView* const rtv = backbuffer->GetOrCreateRenderTargetView();
	m_deviceContext->OMSetRenderTargets( 1, &rtv, nullptr );
}



ID3D11Device* Renderer::GetDevice() const
{
	return m_device;
}



ID3D11DeviceContext* Renderer::GetContext() const
{
	return m_deviceContext;
}



void Renderer::SetBlendMode(BlendMode blendMode)
{
	ASSERT_OR_DIE(m_blendStateByMode[(int) blendMode] != nullptr, "SetBlendMode Failed due to invalid blend state.")
	
	static float const blendConstants[4] = { 0.f, 0.f, 0.f, 0.f };
	m_deviceContext->OMSetBlendState( m_blendStateByMode[(int) blendMode], blendConstants, 0xffffffff );
}



void Renderer::CreateRenderContext()
{
	UINT deviceFlags = 0;
#ifdef _DEBUG
	deviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif
	
	DXGI_SWAP_CHAIN_DESC swapChainDesc = { 0 };
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

	HRESULT hr = D3D11CreateDeviceAndSwapChain(
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

	ID3D11Texture2D* backbufferHandle;
	hr = m_swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**) &backbufferHandle);
	ASSERT_OR_DIE(backbufferHandle != nullptr, "m_swapChain->GetBuffer Failed.")

	m_backbufferTexture = new Texture();
	m_backbufferTexture->WatchInternal(backbufferHandle);

	DX_SAFE_RELEASE(backbufferHandle)
}



void Renderer::DestroyRenderContext()
{
	m_deviceContext->ClearState();
	m_deviceContext->Flush();
	
	if ( m_backbufferTexture )
	{
		m_backbufferTexture->ReleaseResources();
	}
	delete m_backbufferTexture;
	m_backbufferTexture = nullptr;
	
	DX_SAFE_RELEASE( m_swapChain )
	DX_SAFE_RELEASE( m_deviceContext )
	DX_SAFE_RELEASE( m_device )
}



void Renderer::CreateBlendStates()
{
	m_blendStateByMode[(int) BlendMode::Opaque] = CreateBlendState( D3D11_BLEND_ONE, D3D11_BLEND_ZERO, D3D11_BLEND_OP_ADD );
	m_blendStateByMode[(int) BlendMode::Alpha] = CreateBlendState( D3D11_BLEND_SRC_ALPHA, D3D11_BLEND_INV_SRC_ALPHA, D3D11_BLEND_OP_ADD );
	m_blendStateByMode[(int) BlendMode::Additive] = CreateBlendState( D3D11_BLEND_ONE, D3D11_BLEND_ONE, D3D11_BLEND_OP_ADD );
	
	ASSERT_OR_DIE(m_blendStateByMode[(int) BlendMode::Opaque] != nullptr, "BlendMode::Opaque Failed to create.")
	ASSERT_OR_DIE(m_blendStateByMode[(int) BlendMode::Alpha] != nullptr, "BlendMode::Alpha Failed to create.")
	ASSERT_OR_DIE(m_blendStateByMode[(int) BlendMode::Additive] != nullptr, "BlendMode::Additive Failed to create.")
	
	// Default Blend Mode, move to separate default settings function?
	SetBlendMode( BlendMode::Alpha );
}



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
	m_device->CreateBlendState(
		&blendDesc,
		&blendState
	);
	return blendState;
}

	

void Renderer::DestroyBlendStates()
{
	DX_SAFE_RELEASE( m_blendStateByMode[(int) BlendMode::Opaque] )
	DX_SAFE_RELEASE( m_blendStateByMode[(int) BlendMode::Additive] )
	DX_SAFE_RELEASE( m_blendStateByMode[(int) BlendMode::Alpha] )
}
