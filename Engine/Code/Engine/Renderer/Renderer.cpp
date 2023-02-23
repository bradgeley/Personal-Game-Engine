// Bradley Christensen - 2022
#include "Renderer.h"
#include "DefaultShaderSource.h"
#include "RendererInternal.h"
#include "Shader.h"
#include "Texture.h"
#include "Window.h"
#include "Engine/Core/ErrorUtils.h"
#include "VertexBuffer.h"
#include <thread>



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
	CreateDefaultShader();
	CreateBlendStates();
	CreateRasterizerState();
	CreateDepthBuffer();
}

void Renderer::Shutdown()
{
	DestroyBlendStates();
	DestroyRenderContext();
}


void Renderer::BeginFrame()
{
	ClearDepth(1.f);
}



void Renderer::EndFrame()
{
	m_swapChain->Present(0, 0);

	// todo: handle fast framerate issues
	std::this_thread::sleep_for(std::chrono::milliseconds(16));
}



void Renderer::BeginCamera(Camera const& camera)
{
	BindRenderTarget(m_backbufferTexture);

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

	if (!m_currentShader)
	{
		BindShader(m_defaultShader);
	}
}



void Renderer::EndCamera(Camera const& camera)
{
	
}



void Renderer::ClearScreen(Rgba8 const& tint)
{
	float colorAsFloats[4] = {};
	tint.GetAsFloats(&colorAsFloats[0]);
	BindRenderTarget(m_backbufferTexture);
	ID3D11RenderTargetView* const renderTargetView = m_backbufferTexture->CreateOrGetRenderTargetView();
	m_deviceContext->ClearRenderTargetView(renderTargetView, colorAsFloats);
}



void Renderer::DrawVertexBuffer(VertexBuffer* vbo)
{
	BindVertexBuffer( vbo );
	Draw(vbo->GetNumVerts(), 0);
}



void Renderer::BindShader(Shader* shader)
{
	if (shader)
	{
		if (shader == m_currentShader)
		{
			return;
		}
		m_deviceContext->VSSetShader(shader->m_vertexShader, nullptr, 0);
		m_deviceContext->PSSetShader(shader->m_pixelShader, nullptr, 0);
		m_deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		m_currentShader = shader;
	}
	else if (m_defaultShader)
	{
		BindShader(m_defaultShader);
	}
	else
	{
		ERROR_AND_DIE("Could not bind shader, and default didn't exist.")
	}
}



void Renderer::BindRenderTarget(Texture* texture)
{
	ID3D11DepthStencilView* dsv = m_depthBuffer->CreateOrGetDepthStencilView();
	ID3D11RenderTargetView* rtv = texture->CreateOrGetRenderTargetView();
	m_deviceContext->OMSetRenderTargets( 1, &rtv, dsv );
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



void Renderer::Draw(int vertexCount, int vertexOffset)
{
	if (m_isDepthStencilStateDirty)
	{
		UpdateDepthStencilState();
	}
	if (m_isRasterStateDirty)
	{
		UpdateRasterizerState();
	}
	m_deviceContext->Draw(vertexCount, vertexOffset);
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

	ID3D11Texture2D* backbufferHandle;
	result = m_swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**) &backbufferHandle);
	
	ASSERT_OR_DIE(SUCCEEDED(result), "Failed to get swap chain buffer")

	m_backbufferTexture = new Texture();
	m_backbufferTexture->WatchInternal(backbufferHandle);

	DX_SAFE_RELEASE(backbufferHandle)
}



void Renderer::CreateDefaultShader()
{
	ShaderConfig defaultShaderConfig;
	defaultShaderConfig.m_name = "DefaultShader";
	m_defaultShader = new Shader(defaultShaderConfig);
	m_defaultShader->CreateFromSource(s_defaultShaderSource);
}



void Renderer::CreateRasterizerState()
{
	DX_SAFE_RELEASE(m_rasterizerState)

	D3D11_CULL_MODE cullMode = D3D11_CULL_NONE;
	D3D11_FILL_MODE fillMode = D3D11_FILL_SOLID;
	bool windCounterClockwise = true;
	
	D3D11_RASTERIZER_DESC rasterizerDesc;
	rasterizerDesc.FillMode = fillMode;
	rasterizerDesc.CullMode = cullMode;
	rasterizerDesc.FrontCounterClockwise = windCounterClockwise;

	rasterizerDesc.DepthBias = 0;
	rasterizerDesc.DepthBiasClamp = 0.f;
	rasterizerDesc.SlopeScaledDepthBias = 0.f;

	rasterizerDesc.ScissorEnable = false;
	rasterizerDesc.MultisampleEnable = false;
	rasterizerDesc.AntialiasedLineEnable = true;
	rasterizerDesc.DepthClipEnable = true;

	m_device->CreateRasterizerState( &rasterizerDesc, &m_rasterizerState );
}



void Renderer::CreateDepthBuffer()
{
	Window* window = g_window;
	m_depthBuffer = Texture::CreateDepthBuffer(window->GetDimensions());
}



void Renderer::ClearDepth(float depth)
{
	ID3D11DepthStencilView* view = m_depthBuffer->CreateOrGetDepthStencilView();
	m_deviceContext->ClearDepthStencilView( view, D3D11_CLEAR_DEPTH, depth, 0 );
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
	HRESULT result = m_device->CreateBlendState(
		&blendDesc,
		&blendState
	);

	ASSERT_OR_DIE(SUCCEEDED(result), "Failed to create blend state")
	
	return blendState;
}

	

void Renderer::DestroyBlendStates()
{
	DX_SAFE_RELEASE( m_blendStateByMode[(int) BlendMode::Opaque] )
	DX_SAFE_RELEASE( m_blendStateByMode[(int) BlendMode::Additive] )
	DX_SAFE_RELEASE( m_blendStateByMode[(int) BlendMode::Alpha] )
}



void Renderer::UpdateRasterizerState()
{
	DX_SAFE_RELEASE(m_rasterizerState)
	CreateRasterizerState();
	m_deviceContext->RSSetState(m_rasterizerState);
	m_isRasterStateDirty = false;
}



void Renderer::UpdateDepthStencilState()
{
	DX_SAFE_RELEASE(m_depthStencilState)
	
	D3D11_DEPTH_STENCIL_DESC desc;
	desc.DepthEnable = TRUE;
	desc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
	desc.DepthFunc = D3D11_COMPARISON_ALWAYS;
	desc.StencilEnable = FALSE;
	
	m_device->CreateDepthStencilState(&desc, &m_depthStencilState);
	m_deviceContext->OMSetDepthStencilState(m_depthStencilState, 0);
	
	m_isDepthStencilStateDirty = false;
}



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
